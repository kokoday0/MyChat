#include "ChatServer.h"
#include "CfgMgr.h"
int main()
{
	io_server ioserver;
	io_server::work work(ioserver);	
	auto& cfg = CfgMgr::Inst();
	std::string port = cfg["ChatServer1"]["port"];
	std::shared_ptr<ChatServer> chatServer(new ChatServer(ioserver,port));
	chatServer->Start();
	ioserver.run();
	return 0;
}