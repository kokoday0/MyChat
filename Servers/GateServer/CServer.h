#pragma once
#include"const.h"
class CServer:public std::enable_shared_from_this<CServer>
{
public:
	CServer(io_server& io,  short port);
	void Start();
private:
	tcp::acceptor _acceptor;
	io_server& _io;
	tcp::socket _socket;
	std::string _host;
	short _port;
};

