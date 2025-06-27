#pragma once
#include"const.h"
#include"Singleton.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	bool HandleGet(std::string,std::shared_ptr<HttpConnection>);
	bool HandlePost(std::string,std::shared_ptr<HttpConnection>);
private:
	LogicSystem();
	void initialGetHandler();
	void initialPostHandler();
private:
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;
};

