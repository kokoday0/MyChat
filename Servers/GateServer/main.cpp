#include"CServer.h"
#include "test.hpp"
int main() {
	CfgMgr& cfg = CfgMgr::Inst();
	short port = std::stoi(cfg["GateServer"]["port"]);
	try
	{
		io_server io(1);
		boost::asio::io_context::work work(io);
		std::shared_ptr<CServer> server = std::make_shared<CServer>(io, port);
		server->Start();
		io.run();
	}
	catch (std::exception& exp)
	{
		std::cout << "Server Warning , Exception is : " << exp.what() << std::endl;
	}
}