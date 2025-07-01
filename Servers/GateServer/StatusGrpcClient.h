#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
using message::StatusService;
using grpc::Status;
using grpc::ClientContext;
using message::GetChatReq;
using message::GetChatRsp;
class StatusConPool
{
public:
	~StatusConPool();
	StatusConPool(size_t poolsize, std::string& host, std::string& port);
	std::unique_ptr<StatusService::Stub> getConnection();
	void returnConnection(std::unique_ptr<StatusService::Stub> con);
	void Close();
private:
	size_t _poolsize;
	std::string _host;
	std::string _port;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::atomic<bool> _b_stop;
	std::queue<std::unique_ptr<StatusService::Stub>> _pool;
};
class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	GetChatRsp GetChatServer(int uid);
private:
	StatusGrpcClient();
private:
	std::unique_ptr<StatusConPool> _pool;
};

