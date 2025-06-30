#include "MysqlMgr.h"

int MysqlMgr::RegUser(std::string& user, std::string& pwd, std::string& email)
{
	return _dao.RegUser(user,pwd,email);
}

ErrorCodes MysqlMgr::IsExist(std::string& email)
{
	return _dao.IsExist(email);
}

MysqlMgr::MysqlMgr()
{
}
