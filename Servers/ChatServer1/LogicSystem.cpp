#include "LogicSystem.h"
#include "RedisMgr.h"
#include "CfgMgr.h"
bool LogicSystem::VarifyUserLogin(int uid, std::string token)
{
	std::string cache_token;
	std::string query_uid("token_"+ std::to_string(uid));
	bool get_success = RedisMgr::GetInstance()->Get(query_uid,cache_token);
	if (!get_success)
	{
		std::cout << "User Uid Not Found" << std::endl;
		return false;
	}
	if (token != cache_token)
	{
		return false;
	}
	auto& cfg = CfgMgr::Inst();
	auto serverName = cfg["This"]["name"];
	std::string key1("logincount");
	std::string key2(serverName);
	std::string count;
	bool query_success = RedisMgr::GetInstance()->HGet(key1, key2, count);
	if (!query_success)
		return false;
	std::string count_i = std::to_string(std::stoi(count)+1);
	bool add_success = RedisMgr::GetInstance()->HSet(key1, key2, count_i);
	if (!add_success)
		return false;
	return true;
}

bool LogicSystem::initChatStatus()
{
	auto& cfg = CfgMgr::Inst();
	auto serverName = cfg["This"]["name"];
	std::string value("0");
	return RedisMgr::GetInstance()->HSet("logincount", serverName, value);
}

LogicSystem::LogicSystem()
{
	
}
