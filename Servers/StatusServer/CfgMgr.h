#pragma once
#include<unordered_map>
#include<string>
#include<fstream>
class CfgMgr
{
public:
	static CfgMgr& Inst() {
		static CfgMgr mgr;
		return mgr;
	}
	std::unordered_map<std::string, std::string>& operator[](std::string title)
	{
		return _config[title];
	}
private:
	CfgMgr();
	void ReadConfig();
private:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _config;
};

