#include "ChatServer.h"
#include "CfgMgr.h"
void ChatServer::Start()
{
	auto self = shared_from_this();
	std::string target(_host + ":" + _port);
	std::cout << "ChatServer1 Listening on target : " << target << std::endl;
	try
	{
		_acceptor.async_accept(_socket, [self](boost::beast::error_code err)
			{
				if (err)
				{
					std::cout << "Receive Connect Req Error" << std::endl;
					self->Start();
					return;
				}
				std::cout << "Receive Connect Req Success" << std::endl;
				auto& ioserver = self->_pool->getIOService();
				std::shared_ptr<CSession> session(new CSession(ioserver,std::move(self->_socket)));
				session->Start();
			});
	}
	catch (std::exception& exp)
	{
		std::cout << "Acceptor Exception is : " << exp.what() << std::endl;
		self->Start();
	}
	
}
ChatServer::ChatServer(io_server& ioServer) 
	:_ioServer(ioServer),_socket(ioServer), _acceptor(ioServer, tcp::endpoint(tcp::v4(), std::stoi(_port)))
{
	auto& cfg = CfgMgr::Inst();
	_host = cfg["ChatServer1"]["host"];
	_port = cfg["ChatServer1"]["port"];
	_pool.reset(new AsioConPool(5));
}

