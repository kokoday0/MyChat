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


MysqlMgr::MysqlMgr()
{
}
