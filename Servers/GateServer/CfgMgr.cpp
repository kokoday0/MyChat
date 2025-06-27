#include "CfgMgr.h"
CfgMgr::CfgMgr()
{
	ReadConfig();
}
void CfgMgr::ReadConfig()
{
	std::ifstream file("config.ini");
	std::string line;
	std::string title;
	while (std::getline(file, line))
	{
		if (line[0] == '#')
			continue;
		else if (line[0] == '[')
		{
			int end = line.find(']');
			title = line.substr(1, end-1);
			_config[title] = std::unordered_map<std::string,std::string>();
		}
		else
		{
			std::string key = "";
			std::string value = "";
			bool iskey = true;
			for (const auto& i : line)
			{
				if (i == ' ')
					continue;
				if (i == '=')
				{
					iskey = false;
					continue;
				}
				if (iskey)
					key = key + i;
				else
					value = value + i;
			}
			_config[title][key] = value;
		}
	}
	file.close();
}
