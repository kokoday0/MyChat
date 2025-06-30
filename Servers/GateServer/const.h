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
namespace http = beast::http;
using io_server = boost::asio::io_context;
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

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
	EmailError = 1003,
	MysqlError = 1004,
	UserExist = 1005,
};
