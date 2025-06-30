#pragma once
#include<mutex>
#include<queue>
#include<hiredis.h>
#include<atomic>
#include<vector>
#include<iostream>
class RedisConPool
{
public:
	RedisConPool(size_t poolsize,std::string host,int port,const std::string& pwd);
	~RedisConPool();
	redisContext* getConnection();
	void returnConnection(redisContext* connect);
	void Close();
private:
	size_t _poolsize;
	std::string _host;
	int _port;
	std::mutex _mutex;
	std::atomic<bool> _b_stop;
	std::queue<redisContext*> _connections;
	std::condition_variable _cond;
};
