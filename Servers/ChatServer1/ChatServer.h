#pragma once
#include "AsioConPool.h"
class CSession;
class ChatServer : public std::enable_shared_from_this<ChatServer>
{
public:
	void Start();
	ChatServer(io_server& ioServer,std::string port);
	void AddSession(int uid, std::shared_ptr<CSession> session);
	void ClearSession(int uid);
private:
	std::mutex  _session_mutex;
	io_server &_ioServer;
	tcp::acceptor _acceptor;
	tcp::socket _socket;
	std::string _host;
	std::string _port;
	std::unique_ptr<AsioConPool> _pool;
	std::unordered_map<int, std::shared_ptr<CSession>> _sessions;
};

