#include "LogicSystem.h"
#include"HttpConnection.h"
#include"VarifyGrpcClient.h"
bool LogicSystem::HandleGet(std::string s, std::shared_ptr<HttpConnection> con)
{
	if(_get_handlers.find(s) == _get_handlers.end())
		return false;
	_get_handlers[s](con);
	return true;
}

bool LogicSystem::HandlePost(std::string s , std::shared_ptr<HttpConnection> con)
{
	if(_post_handlers.find(s)==_post_handlers.end())
		return false;
	_post_handlers[s](con);
	return true;
}

LogicSystem::LogicSystem()
{
	initialGetHandler();
	initialPostHandler();
}

void LogicSystem::initialGetHandler()
{
	_get_handlers["/get_test"] = [](std::shared_ptr<HttpConnection> con) {
		beast::ostream(con->_response.body()) << "receive get_test req" << std::endl;
		int k = 0;
		for (auto& i : con->_get_params)
		{
			++k;
			beast::ostream(con->_response.body()) << "param" << k << ": key is : " << i.first << ",value is : " << i.second << std::endl;
		}
		};
}

void LogicSystem::initialPostHandler()
{
	_post_handlers["/get_varifycode"] = [this](std::shared_ptr<HttpConnection> con) {
		auto body = con->_request.body().data();
		auto body_str = beast::buffers_to_string(body);
		std::cout << "Receive Get_Varifycode , body is : " << body_str << std::endl;
		con->_response.set(http::field::content_type, "text/json");
		Json::Reader reader;
		Json::Value rvalue;
		Json::Value svalue;
		bool success = reader.parse(body_str, rvalue);
		if (!success)
		{
			std::cout << "Failed to parse Json Data" << std::endl;
			svalue["error"] = ErrorCodes::Error_Json;
			std::string send_str = svalue.toStyledString();
			beast::ostream(con->_response.body()) << send_str;
			return;
		}
		//接收到email，通过VarifyGrpcClient向VarifyGrpcServer发送
		std::string email = rvalue["email"].asString();
		auto grpc_rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
		svalue["error"] = grpc_rsp.error();
		svalue["email"] = rvalue["email"];
		std::string send_str = svalue.toStyledString();
		beast::ostream(con->_response.body()) << send_str;
		};
}
