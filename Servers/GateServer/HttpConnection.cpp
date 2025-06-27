#include "HttpConnection.h"
#include"LogicSystem.h"
#include"UrlEncode.hpp"
HttpConnection::HttpConnection(tcp::socket socket):_socket(std::move(socket))
{
}

void HttpConnection::Start()
{
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, size_t bytes) {
		try
		{
			if (ec)
			{
				self->Start();
				return;
			}
			std::cout << "Http Read Success" << std::endl;
			self->HandleReq();
			self->CheckDeadline();
		}
		catch (std::exception& exp)
		{
			std::cout << "Http Read Fail , Exp is : " << exp.what() << std::endl;
		}
		});
}

void HttpConnection::CheckDeadline()
{
	auto self = shared_from_this();
	_deadline.async_wait([self](beast::error_code ec) {
		if (!ec)
		{
			self->_socket.close();
		}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	http::async_write(_socket, _response, [self](beast::error_code ec,size_t size) {
		if (!ec)
			std::cout << "Write Rsp Success" << std::endl;
		self->_socket.shutdown(tcp::socket::shutdown_send, ec);
		self->_deadline.cancel();
		});
}

void HttpConnection::HandleReq()
{
	//设置版本
	_response.version(_request.version());
	//设置为短链接
	_response.keep_alive(false);
	PreParseGetParam();
	bool success = false;
	if (_request.method() == http::verb::get)
	{
		success = LogicSystem::GetInstance()->HandleGet(_get_url,shared_from_this());
	}
	if (_request.method() == http::verb::post)
	{
		success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
	}
	if (!success)
	{
		_response.result(http::status::not_found);
		_response.set(http::field::content_type, "text/plain");
		beast::ostream(_response.body()) << "url not found\r\n";
		WriteResponse();
		return;
	}
	_response.result(http::status::ok);
	_response.set(http::field::server, "GateServer");
	WriteResponse();
	return;

}

void HttpConnection::PreParseGetParam() {
	// 提取 URI  
	auto uri = _request.target();
	// 查找查询字符串的开始位置（即 '?' 的位置）  
	auto query_pos = uri.find('?');
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}

	_get_url = uri.substr(0, query_pos);
	std::string query_string = uri.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while ((pos = query_string.find('&')) != std::string::npos) {
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); // 假设有 url_decode 函数来处理URL解码  
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	// 处理最后一个参数对（如果没有 & 分隔符）  
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}
