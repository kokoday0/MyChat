#pragma once
#include"const.h"
class AsioConPool
{
public:
	AsioConPool(size_t poolsize);
	io_server& getIOService();
	void Stop();
private:
	size_t _poolsize;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::atomic<bool> _b_stop;
	std::vector<io_server> _services;
	std::vector<std::unique_ptr<work>> _works;
	std::vector<std::thread> _threads;
	int _next_service;
};

