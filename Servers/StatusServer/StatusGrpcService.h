#pragma once
#include"const.h"
using message::GetChatReq;
using message::GetChatRsp;
using grpc::Status;
using grpc::ServerContext;
class StatusGrpcService final: public message::StatusService::Service
{
public:
	StatusGrpcService();
	Status GetChatServer(ServerContext* context, const GetChatReq* req, GetChatRsp* rsp) override;
private:
	std::string getUuid();
	std::string getMinUserChatServer();
};

