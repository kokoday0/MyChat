#include "AsioConPool.h"

AsioConPool::AsioConPool(size_t poolsize)
	:_poolsize(poolsize),_next_service(0),_services(poolsize)
{
	for (size_t i = 0; i < poolsize; ++i)
	{
		_works.emplace_back(new work(_services[i]));
		_threads.emplace_back([this, i]() {
			_services[i].run();
			});
	}
}

io_server& AsioConPool::getIOService()
{
	auto& ioServer = _services[_next_service++];
	_next_service = _next_service % _poolsize;
	return ioServer;
}

void AsioConPool::Stop()
{
	for (auto &i : _threads)
	{
		i.join();
	}
	for (auto& i : _works)
	{
		i->get_io_context().stop();
		i.reset();
	}
}
