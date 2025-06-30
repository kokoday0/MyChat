#include "MysqlDao.h"
#include "CfgMgr.h"
MysqlConPool::~MysqlConPool()
{
	std::unique_lock<std::mutex> lock(_mutex);
	while (!_pool.empty())
	{
		_pool.pop();
	}
}

MysqlConPool::MysqlConPool(size_t poolsize, std::string& host, std::string& port,std::string& user, std::string& pwd,std::string& schema)
	:_poolsize(poolsize),_host(host),_port(port),_b_stop(false)
{
	try {
		for (size_t i = 0; i < _poolsize; ++i)
		{
			auto driver = sql::mysql::get_driver_instance();
			std::unique_ptr<sql::Connection> con(driver->connect(host + ":" + port, user, pwd));
			if (!con)
			{
				std::cout << "Mysql 连接失败" << std::endl;
				continue;
			}
			con->setSchema(schema);
			_pool.push(std::move(con));
		}
	}
	catch (sql::SQLException& exp)
	{
		std::cout << "Mysql Exception is : " << exp.what() << std::endl;
	}
}

std::unique_ptr<sql::Connection> MysqlConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		if (_b_stop)
		{
			return true;
		}
		return !_pool.empty();
		});
	if (_b_stop)
	{
		return nullptr;
	}
	std::unique_ptr<sql::Connection> con = std::move(_pool.front());
	_pool.pop();
	return std::move(con);
}

void MysqlConPool::returnConnection(std::unique_ptr<sql::Connection> con)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_b_stop)
	{
		return;
	}
	_pool.push(std::move(con));
	_cond.notify_one();
}

void MysqlConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

MysqlDao::MysqlDao()
{
	auto& cfg = CfgMgr::Inst();
	std::string user = cfg["Mysql"]["user"];
	std::string passwd = cfg["Mysql"]["passwd"];
	std::string host = cfg["Mysql"]["host"];
	std::string schema = cfg["Mysql"]["schema"];
	std::string port = cfg["Mysql"]["port"];
	_pool.reset(new MysqlConPool(5, host, port, user, passwd, schema));
}

MysqlDao::~MysqlDao()
{
	_pool->Close();
}

int MysqlDao::RegUser(std::string& user, std::string& pwd, std::string& email)
{
	auto con = _pool->getConnection();
	if (!con)
	{
		return -1;
	}
	Defer defer([this, &con]() {
		_pool->returnConnection(std::move(con));
		});
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("INSERT INTO users(user,passwd,email) VALUE(?,?,?)"));
		stmt->setString(1, user);
		stmt->setString(2, pwd);
		stmt->setString(3, email);
		stmt->executeUpdate();//返回update行数
		std::unique_ptr<sql::PreparedStatement> stmt_query(con->prepareStatement("SELECT uid FROM users WHERE email = ?"));
		stmt_query->setString(1, email);
		std::unique_ptr<sql::ResultSet> res(stmt_query->executeQuery());
		while (res->next())
		{
			_pool->returnConnection(std::move(con));
			return res->getInt("uid");
		}
		return -1;
	}
	catch (sql::SQLException& exp)
	{
		std::cout << "RegUser Failed , SqlException is : " << exp.what() << std::endl;
		return -1;
	}
}

ErrorCodes MysqlDao::IsExist(std::string& email)
{
	auto con = _pool->getConnection();
	if (!con)
	{
		return ErrorCodes::MysqlError;
	}
	Defer defer([this, &con]() {
		_pool->returnConnection(std::move(con));
		});
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT * FROM users WHERE email = ?"));
		stmt->setString(1,email);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
		while (res->next())
			return ErrorCodes::UserExist;
		return ErrorCodes::Success;
	}
	catch (sql::SQLException& exp)
	{
		std::cerr << "Query Exist emial : " << email << " Failed , SqlException is : " << exp.what() << std::endl;
		return ErrorCodes::MysqlError;
	}
}
