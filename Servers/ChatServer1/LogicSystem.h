#pragma once
#include "Singleton.h"
#include"const.h"

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	bool VarifyUserLogin(int uid, std::string token);
	bool initChatStatus();
	bool ClearUser(int uid);
	ErrorCodes GetUserInfo(int uid, UserInfo& user);
private:
	LogicSystem();
};

