#include "StatusGrpcService.h"
#include"RedisMgr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include"CfgMgr.h"
StatusGrpcService::StatusGrpcService()
{
}

Status StatusGrpcService::GetChatServer(ServerContext* context, const GetChatReq* req, GetChatRsp* rsp)
{
	int uid = req->uid();
	std::string token = getUuid();
	std::string key = "token_" + std::to_string(uid);
	bool success = RedisMgr::GetInstance()->Set(key, token);
	if (!success)
	{
		rsp->set_error(ErrorCodes::RPCFailed);
		return Status::CANCELLED;
	}
	auto& cfg = CfgMgr::Inst();
	auto minServer = getMinUserChatServer();
	rsp->set_error(ErrorCodes::Success);
	rsp->set_host(cfg[minServer]["host"]);
	rsp->set_port(cfg[minServer]["port"]);
	rsp->set_token(token);
	return Status::OK;
}

std::string StatusGrpcService::getUuid()
{
	boost::uuids::random_generator gen;
	auto token = gen();
	return boost::uuids::to_string(token);
}

std::string StatusGrpcService::getMinUserChatServer()
{
	std::string minServer = "ChatServer1";
	auto& cfg = CfgMgr::Inst();
	int serverNum = std::stoi(cfg["ChatServers"]["number"]);
	int min = 9999;
	for (int i = 0; i < serverNum; ++i)
	{
		std::string serverName = cfg["ChatServers"]["chatserver" + std::to_string(i + 1)];
		std::string key("logincount");
		std::string value;
		bool success = RedisMgr::GetInstance()->HGet(key, serverName, value);
		if (!success)
		{
			continue;
		}
		int count = std::stoi(value);
		if (min > count)
		{
			minServer = serverName;
			min = count;
		}
	}
	return minServer;
}
