#include "StatusServer.h"
#include "CfgMgr.h"
void StatusServer::Start()
{
	auto& cfg = CfgMgr::Inst();
	std::string target(cfg["StatusServer"]["host"] + ":" + cfg["StatusServer"]["port"]);
	std::cout << "StatusServer Listening on : " << target << std::endl;
	_builder.AddListeningPort(target, grpc::InsecureServerCredentials());
	_builder.RegisterService(&_service);
	std::unique_ptr<grpc::Server> server(_builder.BuildAndStart());
	_server = move(server);
	_server->Wait();
}

StatusServer::StatusServer()
{
	_server.reset();
}
