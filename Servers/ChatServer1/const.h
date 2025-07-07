#pragma once
#include<iostream>
#include<string>
#include<map>
#include<memory>
#include<functional>
#include<unordered_map>
#include<boost/beast/http.hpp>
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include<thread>
#include<queue>
namespace beast = boost::beast;
using io_server = boost::asio::io_context;
using work = boost::asio::io_context::work;
using tcp = boost::asio::ip::tcp;

class Defer {
public:
	Defer(std::function<void()> func):_func(func) {};
	~Defer() {
		_func();
	}
private:
	std::function<void()> _func;
};

struct UserInfo {
	Json::Value _info;
	Json::Value _data;
	UserInfo(){}
	UserInfo(Json::Value info, Json::Value data) :_info(info), _data(data) {}
};

enum ReqId {
	ID_GET_VARIFY_CODE = 1001,
	ID_REG_USER = 1002,
	ID_USER_LOGIN = 1003,
};
enum RspId {
	ID_LOGIN_RSP = 0,
};
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
	EmailError = 1003,
	MysqlError = 1004,
	UserExist = 1005,
	UserNotExist = 1006,
	PasswdError = 1007,
	UserLogined = 1008,
	StatusError = 1009,
};
