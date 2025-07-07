#include "ChatServer.h"
#include "CfgMgr.h"
#include "CSession.h"
#include "LogicSystem.h"
void ChatServer::Start()
{
	auto self = shared_from_this();
	std::string target(_host + ":" + _port);
	std::cout << "ChatServer1 Listening on target : " << target << std::endl;
	LogicSystem::GetInstance()->initChatStatus();
	try
	{
		auto& ioserver = self->_pool->getIOService();
		std::shared_ptr<CSession> session(new CSession(ioserver, std::move(self->_socket)));
		_acceptor.async_accept(session->GetSocket(), [self,session](boost::beast::error_code err)
			{
				if (err)
				{
					std::cout << "Receive Connect Req Error" << std::endl;
					self->Start();
					return;
				}
				std::cout << "Receive Connect Req Success" << std::endl;
				session->Start(self.get());
			});
	}
	catch (std::exception& exp)
	{
		std::cout << "Acceptor Exception is : " << exp.what() << std::endl;
		self->Start();
	}
	
}
ChatServer::ChatServer(io_server& ioServer,std::string port) 
	:_ioServer(ioServer),_socket(ioServer),_port(port), _acceptor(ioServer, tcp::endpoint(tcp::v4(), std::stoi(port)))
{
	_pool.reset(new AsioConPool(5));
}

void ChatServer::AddSession(int uid, std::shared_ptr<CSession> session)
{
	std::lock_guard<std::mutex> lock(_session_mutex);
	_sessions[uid] = session;
}

void ChatServer::ClearSession(int uid)
{
	std::lock_guard<std::mutex> lock(_session_mutex);
	_sessions.erase(uid);
	bool success = LogicSystem::GetInstance()->ClearUser(uid);
	if (!success)
	{
		std::cout << "User Exit Failed" << std::endl;
		return;
	}
	std::cout << "User " << uid << " Exit Success" << std::endl;
}

