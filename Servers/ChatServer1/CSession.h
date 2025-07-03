#pragma once
#include "const.h"
class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(io_server& ioServer,tcp::socket socket);
	void Start();
private:
	io_server& _ioServer;
	tcp::socket _socket;
};

