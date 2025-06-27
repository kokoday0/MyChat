#pragma once
#include"VarifyImplService.h"
class VarifyGrpcServer : public Singleton<VarifyGrpcServer>
{
	friend class Singleton<VarifyGrpcServer>;
public:
	void Start();
private:
	VarifyGrpcServer();
private:
	grpc::ServerBuilder _builder;
	VarifyImplService _service;
	std::unique_ptr<grpc::Server> _server;
};

