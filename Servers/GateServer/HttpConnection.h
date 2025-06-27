#pragma once
#include"const.h"
class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(tcp::socket socket);
	void Start();
private:
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	void PreParseGetParam();
private:
	beast::flat_buffer _buffer{ 8192 };
	tcp::socket _socket;
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	boost::asio::steady_timer _deadline{ _socket.get_executor(),std::chrono::seconds(60) };

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

