#pragma once
#include<string>
#include"Singleton.h"
#include<hiredis.h>
#include<iostream>
#include"RedisConPool.h"
class RedisMgr:public Singleton<RedisMgr>,
	public std::enable_shared_from_this<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	bool Get(const std::string& key, std::string& value);
	bool Set(const std::string& key, const std::string& value);
	bool Del(const std::string& key);
	bool ExistKey(const std::string& key);
	void SetDeleteTime(const std::string& key,int time);
	void Close();
private:
	RedisMgr();
private:
	std::unique_ptr<RedisConPool> _pool;
	redisReply* _reply;
};

