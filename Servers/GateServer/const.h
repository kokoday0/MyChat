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
namespace beast = boost::beast;
namespace http = beast::http;
using io_server = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
};