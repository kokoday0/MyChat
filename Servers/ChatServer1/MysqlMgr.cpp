#include "MysqlMgr.h"

int MysqlMgr::RegUser(std::string& user, std::string& pwd, std::string& email)
{
	return _dao.RegUser(user,pwd,email);
}

ErrorCodes MysqlMgr::IsExist(std::string& email)
{
	return _dao.IsExist(email);
}

ErrorCodes MysqlMgr::CheckLogin(const std::string& user, const std::string& passwd,int& uid)
{
	return _dao.CheckLogin(user, passwd,uid);
}

ErrorCodes MysqlMgr::GetUser(int uid,Json::Value& info)
{
	return _dao.GetUser(uid,info);
}

ErrorCodes MysqlMgr::GetChatLog(int uid, Json::Value& data)
{
	return _dao.GetChatLog(uid,data);
}


MysqlMgr::MysqlMgr()
{
}
