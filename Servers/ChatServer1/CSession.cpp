#include "CSession.h"

CSession::CSession(io_server& ioServer,tcp::socket socket):_ioServer(ioServer),_socket(std::move(socket))
{
	
}

void CSession::Start()
{
}
