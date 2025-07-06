#include "CSession.h"
#include "LogicSystem.h"
#include "ChatServer.h"
int HEAD_ID_LEN = 2;
int HEAD_LEN_LEN = 2;
CSession::CSession(io_server& ioServer,tcp::socket&& socket):_ioServer(ioServer),_socket(std::move(socket))
{
	initHandlers();
}

void CSession::Start(std::shared_ptr<ChatServer> server)
{
	_server = server;
	//
	ReadHead();
}

int CSession::GetUid()
{
	return _uid;
}

void CSession::ReadHead()
{
	ReadLen(0, HEAD_ID_LEN + HEAD_LEN_LEN, [this](boost::system::error_code err, size_t size) {
		if (err || size < 4)
		{
			std::cout << "Read MsgHead Failed" << ",Error is : " << err.what() << std::endl;
			return;
		}
		short id = 0;
		short len = 0;
		memcpy(&id, _data, 2);
		memcpy(&len, _data + 2, 2);
		id = boost::asio::detail::socket_ops::network_to_host_short(id);
		len = boost::asio::detail::socket_ops::network_to_host_short(len);
		_msg_id = static_cast<ReqId>(id);
		_msg_len = len;
		if (_id_handlers.find(_msg_id) == _id_handlers.end())
		{
			std::cout << "ReqId : " << _msg_id << " is Invalid" << std::endl;
			ReadHead();
			return;
		}
		_id_handlers[_msg_id]();
		ReadHead();
		});
}

void CSession::ReadLen(int readLen,int totalLen,handler func)
{
	auto self = shared_from_this();
	_socket.async_read_some(boost::asio::buffer(_data + readLen, totalLen - readLen), [self,readLen,totalLen,func](boost::system::error_code err,size_t size) {
		if (err == boost::asio::error::eof)
		{
			self->_server->ClearSession(self->_uid);
			return;
		}
		if (err)
		{
			func(err,readLen + size);
			return;
		}
		if (readLen + size >= totalLen)
		{
			func(err, readLen + size);
			return;
		}
		self->ReadLen(readLen + size, totalLen, func);
		});
}

void CSession::ReadBody()
{
	ReadLen(HEAD_ID_LEN + HEAD_LEN_LEN, _msg_len + HEAD_ID_LEN + HEAD_LEN_LEN, [](boost::system::error_code err , size_t size) {

		});
}

void CSession::initHandlers()
{
	_id_handlers[ReqId::ID_USER_LOGIN] = [&]() {
		ReadBody();
		Json::Reader reader;
		Json::Value rvalue;
		std::string data_str(_data + HEAD_ID_LEN + HEAD_LEN_LEN, _msg_len);
		std::cout << "Body str is : " << data_str << std::endl;
		bool success = reader.parse(data_str, rvalue);
		if (!success)
		{
			std::cout << "Post Data Body Failed" << std::endl;
			return;
		}
		int uid = rvalue["uid"].asInt();
		std::string token = rvalue["token"].asString();
		bool match = LogicSystem::GetInstance()->VarifyUserLogin(uid, token);
		if (!match)
		{
			return;
		}
		_uid = uid;
		_server->AddSession(uid, shared_from_this());
		};

}

void CSession::Close()
{
	_socket.close();
}
