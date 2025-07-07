#pragma once
#include"Singleton.h"
#include"MysqlDao.h"
class MysqlMgr : public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	int RegUser(std::string& user, std::string& pwd, std::string& email);
	ErrorCodes IsExist(std::string& email);
	ErrorCodes CheckLogin(const std::string& user, const std::string& passwd,int& uid);
	ErrorCodes GetUser(int uid,Json::Value& info);
	ErrorCodes GetChatLog(int uid, Json::Value& data);
private:
	MysqlMgr();
private:
	MysqlDao _dao;
};

