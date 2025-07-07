#include "CSession.h"
#include "LogicSystem.h"
#include "ChatServer.h"
int HEAD_ID_LEN = 2;
int HEAD_LEN_LEN = 2;
CSession::CSession(io_server& ioServer,tcp::socket&& socket):_ioServer(ioServer),_socket(std::move(socket))
{
	initHandlers();
}

void CSession::Start(ChatServer* server)
{
	_server = server;
	//
	ReadHead();
}

int CSession::GetUid()
{
	return _uid;
}

tcp::socket& CSession::GetSocket()
{
	return _socket;
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
			self->Close();
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

void CSession::Send(std::string data, short msg_id)
{
	short msg_len = data.size();
	char msg_head[4];
	memcpy(msg_head, &msg_id, HEAD_ID_LEN);
	memcpy(msg_head+HEAD_ID_LEN, &msg_len, HEAD_LEN_LEN);
	std::lock_guard<std::mutex> lock(_mutex);
	_msg_queue.emplace(msg_head + data);
	auto self = shared_from_this();
	try {
		while (!_msg_queue.empty())
		{
			auto send_data = _msg_queue.front();
			_msg_queue.pop();
			boost::asio::async_write(_socket, boost::asio::buffer(send_data, send_data.size()), [self, send_data](boost::system::error_code err, size_t size) {
				if (err)
				{
					std::cout << "Write Data Failed" << std::endl;
					return;
				}
				});
		}
	}
	catch (std::exception& exp)
	{
		std::cout << "Write Failed,Exception is : " << exp.what() << std::endl;
		return;
	}
}

void CSession::initHandlers()
{
	_id_handlers[ReqId::ID_USER_LOGIN] = [&]() {
		ReadBody();
		Json::Reader reader;
		Json::Value rvalue;
		Json::Value svalue;
		Defer defer([&svalue,this]() {
			Send(svalue.toStyledString(), RspId::ID_LOGIN_RSP);
			});
		std::string data_str(_data + HEAD_ID_LEN + HEAD_LEN_LEN, _msg_len);
		std::cout << "Body str is : " << data_str << std::endl;
		bool success = reader.parse(data_str, rvalue);
		if (!success)
		{
			svalue["error"] = ErrorCodes::Error_Json;
			std::cout << "Post Data Body Failed" << std::endl;
			return;
		}
		int uid = rvalue["uid"].asInt();
		std::string token = rvalue["token"].asString();
		bool match = LogicSystem::GetInstance()->VarifyUserLogin(uid, token);
		if (!match)
		{
			svalue["error"] = ErrorCodes::StatusError;
			return;
		}
		_uid = uid;
		_server->AddSession(uid, shared_from_this());
		//»Ø°ü
		UserInfo user;
		ErrorCodes userRes = LogicSystem::GetInstance()->GetUserInfo(uid, user);
		if (userRes != ErrorCodes::Success)
		{
			svalue["error"] = userRes;
			return;
		}
		svalue["error"] = userRes;
		svalue["info"] = user._info;
		svalue["data"] = user._data;
		};

}

void CSession::Close()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_socket.close();
}
