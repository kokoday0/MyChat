#pragma once
#include "const.h"

class ChatServer;
typedef std::function<void(boost::system::error_code err,size_t size)> handler;
typedef std::function<void()> id_handler;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(io_server& ioServer,tcp::socket&& socket);
	void Start(std::shared_ptr<ChatServer> server);
	int GetUid();
private:
	void ReadHead();
	void ReadLen(int readLen,int totalLen,handler func);
	void ReadBody();
	void initHandlers();
	void Close();
private:
	io_server& _ioServer;
	tcp::socket _socket;
	std::shared_ptr<ChatServer> _server;
	char _data[8192];
	ReqId _msg_id;
	short _msg_len;
	std::unordered_map<ReqId, id_handler> _id_handlers;
	int _uid;
};

