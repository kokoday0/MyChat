#pragma once
#include"Singleton.h"
#include"MysqlDao.h"
class MysqlMgr : public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	int RegUser(std::string& user, std::string& pwd, std::string& email);
	ErrorCodes IsExist(std::string& email);
private:
	MysqlMgr();
private:
	MysqlDao _dao;
};

