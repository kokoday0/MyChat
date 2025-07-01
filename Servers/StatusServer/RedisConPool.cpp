#include "RedisConPool.h"

RedisConPool::RedisConPool(size_t poolsize, std::string host, int port,const std::string& pwd)
	:_poolsize(poolsize),_host(host),_port(port)
{
	for (size_t i = 0; i < poolsize; ++i)
	{
		redisContext* connect = redisConnect(host.c_str(), port);
		if (!connect || connect->err != 0)
		{
			if (connect)
				redisFree(connect);
			continue;
		}
		redisReply* reply = (redisReply*)redisCommand(connect, "AUTH %s", pwd.c_str());
		if (!reply || reply->type == REDIS_REPLY_ERROR)
		{
			std::cout << "Redis Auth Falied" << std::endl;
			freeReplyObject(reply);
			continue;
		}
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		_connections.push(connect);
	}
}

RedisConPool::~RedisConPool()
{
	std::unique_lock<std::mutex> lock(_mutex);
	while (!_connections.empty())
	{
		auto* connect = _connections.front();
		_connections.pop();
		redisFree(connect);
	}
}

redisContext* RedisConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		if (_b_stop)
			return true;
		return !_connections.empty();
		});
	if (_b_stop)
		return nullptr;
	auto* connect = _connections.front();
	_connections.pop();
	return connect;
}

void RedisConPool::returnConnection(redisContext* connect)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_b_stop)
	{
		redisFree(connect);
		return;
	}
	_connections.push(connect);
	_cond.notify_one();
}

void RedisConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}
