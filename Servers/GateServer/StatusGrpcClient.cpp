#include "StatusGrpcClient.h"
#include "CfgMgr.h"
StatusConPool::~StatusConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (!_pool.empty())
		_pool.pop();
}

StatusConPool::StatusConPool(size_t poolsize, std::string& host, std::string& port)
	:_poolsize(poolsize),_host(host),_port(port)
{
	for (size_t i = 0; i < _poolsize; ++i)
	{
		std::string target(host+":" + port);
		auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
		auto stub = StatusService::NewStub(channel);
		_pool.push(move(stub));
	}
}

std::unique_ptr<StatusService::Stub> StatusConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		if (_b_stop)
			return true;
		return !_pool.empty();
		});
	if (_b_stop)
		return nullptr;
	auto con = move(_pool.front());
	_pool.pop();
	return move(con);
}

void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> con)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_b_stop)
		return;
	_pool.push(move(con));
	_cond.notify_one();
}

void StatusConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

GetChatRsp StatusGrpcClient::GetChatServer(int uid)
{
	GetChatRsp rsp;
	GetChatReq req;
	ClientContext context;
	auto con = _pool->getConnection();
	if (!con)
	{
		rsp.set_error(ErrorCodes::RPCFailed);
		std::cout << "StatusGrpcClient Get Connection Failed" << std::endl;
		return rsp;
	}
	Defer defer([this, &con]() {
		_pool->returnConnection(move(con));
		});
	req.set_uid(uid);
	auto status = con->GetChatServer(&context, req, &rsp);
	if (!status.ok())
	{
		rsp.set_error(ErrorCodes::RPCFailed);
	}
	return rsp;
}

StatusGrpcClient::StatusGrpcClient()
{
	auto& cfg = CfgMgr::Inst();
	auto host = cfg["StatusServer"]["host"];
	auto port = cfg["StatusServer"]["port"];
	_pool.reset(new StatusConPool(5, host, port));
}
