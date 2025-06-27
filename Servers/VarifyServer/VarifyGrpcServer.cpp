#include "VarifyGrpcServer.h"
#include"CfgMgr.h"
void VarifyGrpcServer::Start()
{
	CfgMgr& cfg = CfgMgr::Inst();
	std::string target(cfg["VarifyGrpcServer"]["host"] + ":" + cfg["VarifyGrpcServer"]["port"]);
	std::cout << "Start Listening on " << target << std::endl;
	_builder.AddListeningPort(target, grpc::InsecureServerCredentials());
	_builder.RegisterService(&_service);
	std::unique_ptr<grpc::Server> server(_builder.BuildAndStart());
	_server = std::move(server);
	_server->Wait();
}
VarifyGrpcServer::VarifyGrpcServer()
{
	_server = nullptr;
}
