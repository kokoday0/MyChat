#include "ChatServer.h"
int main()
{
	io_server ioserver;
	io_server::work work(ioserver);
	std::unique_ptr<ChatServer> chatServer(new ChatServer(ioserver));
	chatServer->Start();
	ioserver.run();
	return 0;
}