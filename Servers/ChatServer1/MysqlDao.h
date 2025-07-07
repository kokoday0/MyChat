#pragma once
#include"const.h"
#include<jdbc/mysql_driver.h>
#include<jdbc/mysql_connection.h>
#include<jdbc/cppconn/prepared_statement.h>
#include<jdbc/cppconn/exception.h>
#include<jdbc/cppconn/resultset.h>
#include<jdbc/cppconn/statement.h>

class MysqlConPool {
public:
	~MysqlConPool();
	MysqlConPool(size_t poolsize,std::string& host,std::string& port,std::string& user,std::string& pwd,std::string& schema);
	std::unique_ptr<sql::Connection> getConnection();
	void returnConnection(std::unique_ptr<sql::Connection> con);
	void Close();
private:
	size_t _poolsize;
	std::string _host;
	std::string _port;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::atomic<bool> _b_stop;
	std::queue<std::unique_ptr<sql::Connection>> _pool;
};
class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(std::string& user, std::string& pwd, std::string& email);
	ErrorCodes IsExist(std::string& email);
	ErrorCodes CheckLogin(const std::string& user, const std::string& passwd, int& uid);
	ErrorCodes GetUser(int uid,Json::Value& info);
	ErrorCodes GetChatLog(int uid, Json::Value& data);
private:
	std::unique_ptr<MysqlConPool> _pool;
};

