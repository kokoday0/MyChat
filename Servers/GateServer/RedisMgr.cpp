#include "RedisMgr.h"
#include"CfgMgr.h"
RedisMgr::~RedisMgr()
{
    _pool->Close();
}
bool RedisMgr::Get(const std::string& key, std::string& value)
{
    auto* _connect = _pool->getConnection();
    if (!_connect)
    {
        std::cout << "RedisConPool is Closed" << std::endl;
        return false;
    }
    _reply = (redisReply*)redisCommand(_connect, "GET %s", key.c_str());
    if (_reply->type != REDIS_REPLY_STRING)
    {
        std::cout << "Redis [Get : " << key << "] Failed" << std::endl;
        freeReplyObject(_reply);
        _pool->returnConnection(_connect);
        return false;
    }
    value = _reply->str;
    freeReplyObject(_reply);
    _pool->returnConnection(_connect);
    std::cout << "Redis [Get : " << key << "] Success,Value is : " << value << std::endl;
    return true;
}

bool RedisMgr::Set(const std::string& key, const std::string& value)
{
    auto* _connect = _pool->getConnection();
    if (!_connect)
    {
        std::cout << "RedisConPool is Closed" << std::endl;
        return false;
    }
    _reply = (redisReply*)redisCommand(_connect, "SET %s %s", key.c_str(), value.c_str());
    if (!_reply ||_reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "Redis [Set : " << key << " - " << value << "] Failed" << std::endl;
        freeReplyObject(_reply);
        _pool->returnConnection(_connect);
        return false;
    }
    std::cout << "Redis [Set : " << key << " - " << value << "] Success" << std::endl;
    freeReplyObject(_reply);
    _pool->returnConnection(_connect);
    return true;
}

bool RedisMgr::Del(const std::string& key)
{
    auto* _connect = _pool->getConnection();
    if (!_connect)
    {
        std::cout << "RedisConPool is Closed" << std::endl;
        return false;
    }
    _reply = (redisReply*)redisCommand(_connect, "DEL %s", key.c_str());
    if (!_reply || _reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "Redis [Del : " << key << "] Failed" << std::endl;
        freeReplyObject(_reply);
        _pool->returnConnection(_connect);
        return false;
    }
    std::cout << "Redis [Del : " << key << "] Success" << std::endl;
    freeReplyObject(_reply);
    _pool->returnConnection(_connect);
    return true;
}

bool RedisMgr::ExistKey(const std::string& key)
{
    auto* _connect = _pool->getConnection();
    if (!_connect)
    {
        std::cout << "RedisConPool is Closed" << std::endl;
        return false;
    }
    _reply = (redisReply*)redisCommand(_connect, "exists %s", key.c_str());
    if (!_reply || _reply->type != REDIS_REPLY_INTEGER || _reply->integer == 0)
    {
        std::cout << "Redis [ExistKey : " << key << "] Not Found" << std::endl;
        freeReplyObject(_reply);
        _pool->returnConnection(_connect);
        return false;
    }
    std::cout << "Redis [ExistKey : " << key << "] Existed" << std::endl;
    freeReplyObject(_reply);
    _pool->returnConnection(_connect);
    return true;
}

void RedisMgr::SetDeleteTime(const std::string& key, int time)
{
    auto* _connect = _pool->getConnection();
    if (!_connect)
    {
        std::cout << "RedisConPool is Closed" << std::endl;
        return;
    }
    std::string command("EXPIRE " + key + " " + std::to_string(time));
    _reply = (redisReply*)redisCommand(_connect, command.c_str());
    if (!_reply || _reply->type != REDIS_REPLY_INTEGER || _reply->integer == 0)
    {
        std::cout << "Failed to [Set " << key << " TTL ]" << std::endl;
        if (_reply) freeReplyObject(_reply);
        return;
    }
    freeReplyObject(_reply);
    _pool->returnConnection(_connect);
}

void RedisMgr::Close()
{
    _pool->Close();
}

RedisMgr::RedisMgr()
{
    auto& cfg = CfgMgr::Inst();
    std::string host = cfg["Redis"]["host"];
    int port = std::stoi(cfg["Redis"]["port"]);
    std::string pwd = cfg["Redis"]["passwd"];
    _pool.reset(new RedisConPool(5, host, port, pwd));
}
