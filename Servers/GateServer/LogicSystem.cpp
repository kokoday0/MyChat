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
	//��ȡ��֤��
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
		//���յ�email��ͨ��VarifyGrpcClient��VarifyGrpcServer����
		std::string email = rvalue["email"].asString();
		auto grpc_rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
		svalue["error"] = grpc_rsp.error();
		svalue["email"] = rvalue["email"];
		std::string send_str = svalue.toStyledString();
		beast::ostream(con->_response.body()) << send_str;
		};
	//�û�ע��
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
		//��ȡ������Ϣ
		std::string user = rvalue["user"].asString();
		std::string passwd = rvalue["passwd"].asString();
		std::string varifycode = rvalue["varifycode"].asString();
		std::string email = rvalue["email"].asString();
		//��֤�û��Ƿ���ڣ���֤���Ƿ�ƥ��
		svalue["email"] = rvalue["email"];
		svalue["user"] = rvalue["user"];
		svalue["error"] = ErrorCodes::Success;
		//�û��Ƿ����
		ErrorCodes err = MysqlMgr::GetInstance()->IsExist(email);
		if (err != Success)
		{
			svalue["error"] = err;
			return;
		}
		//��֤���Ƿ�ƥ��
		std::string varicode_cache = "";
		bool query_success = RedisMgr::GetInstance()->Get(email, varicode_cache);
		if (!query_success || varicode_cache != varifycode)
		{
			std::cout << "Register Email Not Found" << std::endl;
			svalue["error"] = ErrorCodes::EmailError;
			return;
		}
		//����Mysql
		int uid = MysqlMgr::GetInstance()->RegUser(user, passwd, email);
		if (uid == -1)
		{
			svalue["error"] = ErrorCodes::MysqlError;
		}
		svalue["uid"] = uid;
		};

	//�û���¼
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
		//��ѯ���ݿ��˺������Ƿ�ƥ��
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
		//�Ƿ��ѵ�¼
		std::string key("token_" + std::to_string(uid));
		bool hasLogined = RedisMgr::GetInstance()->ExistKey(key);
		if (hasLogined)
		{
			std::cout << "User Has Logined" << std::endl;
			svalue["error"] = ErrorCodes::UserLogined;
			return;
		}
		//ƥ�����StatusServer��ȡĿ��ChatServer��token
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


