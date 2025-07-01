#include "LogicSystem.h"
#include"HttpConnection.h"
#include"VarifyGrpcClient.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"

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
	//获取验证码
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
	//用户注册
	_post_handlers["/user_register"] = [this](std::shared_ptr<HttpConnection> con) {
		auto body = con->_request.body();
		auto body_str = beast::buffers_to_string(body.data());
		std::cout << "Receive user_register , body is : " << body_str << std::endl;
		Json::Value rvalue;
		Json::Value svalue;
		Json::Reader reader;
		Defer defer([&body_str, &con,&svalue]() {
			beast::ostream(con->_response.body()) << svalue.toStyledString();
			});
		con->_response.set(http::field::content_type, "text/json");
		reader.parse(body_str, rvalue);
		//读取接收信息
		std::string user = rvalue["user"].asString();
		std::string passwd = rvalue["passwd"].asString();
		std::string varifycode = rvalue["varifycode"].asString();
		std::string email = rvalue["email"].asString();
		//验证用户是否存在，验证码是否匹配
		svalue["email"] = rvalue["email"];
		svalue["user"] = rvalue["user"];
		svalue["error"] = ErrorCodes::Success;
		//用户是否存在
		ErrorCodes err = MysqlMgr::GetInstance()->IsExist(email);
		if (err != Success)
		{
			svalue["error"] = err;
			return;
		}
		//验证码是否匹配
		std::string varicode_cache = "";
		bool query_success = RedisMgr::GetInstance()->Get(email, varicode_cache);
		if (!query_success || varicode_cache != varifycode)
		{
			std::cout << "Register Email Not Found" << std::endl;
			svalue["error"] = ErrorCodes::EmailError;
			return;
		}
		//存入Mysql
		int uid = MysqlMgr::GetInstance()->RegUser(user, passwd, email);
		if (uid == -1)
		{
			svalue["error"] = ErrorCodes::MysqlError;
		}
		svalue["uid"] = uid;
		};

	//用户登录
	_post_handlers["/user_login"] = [this](std::shared_ptr<HttpConnection> con) {
		auto body = con->_request.body();
		auto body_str = beast::buffers_to_string(body.data());
		std::cout << "Received user_login Request" << std::endl;
		Json::Value rvalue;
		Json::Value svalue;
		Json::Reader reader;
		Defer defer([&svalue, &con]() {
			beast::ostream(con->_response.body()) << svalue.toStyledString();
			});
		bool success = reader.parse(body_str, rvalue);
		if (!success)
		{
			std::cout << "Parse Json Failed" << std::endl;
			svalue["error"] = ErrorCodes::Error_Json;
			return;
		}
		svalue["user"] = rvalue["user"];
		//查询数据库账号密码是否匹配
		int uid = -1;
		std::string user = rvalue["user"].asString();
		std::string passwd = rvalue["passwd"].asString();
		ErrorCodes login_query = MysqlMgr::GetInstance()->CheckLogin(user, passwd, uid);
		svalue["error"] = login_query;
		if (login_query != ErrorCodes::Success)
		{
			std::cout << "Login Failed" << std::endl;
			return;
		}
		//是否已登录
		std::string key("token_" + std::to_string(uid));
		bool hasLogined = RedisMgr::GetInstance()->ExistKey(key);
		if (hasLogined)
		{
			std::cout << "User Has Logined" << std::endl;
			svalue["error"] = ErrorCodes::UserLogined;
			return;
		}
		//匹配后向StatusServer获取目标ChatServer和token
		GetChatRsp rsp = StatusGrpcClient::GetInstance()->GetChatServer(uid);
		if (rsp.error() != ErrorCodes::Success)
		{
			svalue["error"] = ErrorCodes::RPCFailed;
			return;
		}
		std::cout << "Login Success" << std::endl;
		std::string token = rsp.token();
		std::string host = rsp.host();
		std::string port = rsp.port();
		svalue["token"] = token;
		svalue["uid"] = uid;
		svalue["host"] = host;
		svalue["port"] = port;
		};
}


