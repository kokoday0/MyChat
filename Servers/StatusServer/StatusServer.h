#pragma once
#include "StatusGrpcService.h"
#include "Singleton.h"
class StatusServer : public Singleton<StatusServer>
{
	friend class Singleton<StatusServer>;
public:
	void Start();
private:
	StatusServer();
private:
	StatusGrpcService _service;
	grpc::ServerBuilder _builder;
	std::unique_ptr<grpc::Server> _server;

};

