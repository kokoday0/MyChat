#pragma once
#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "grpcpp/grpcpp.h"
#include <condition_variable>
class Defer {
public:
	Defer(std::function<void()> func) :_func(func) {};
	~Defer() {
		_func();
	}
private:
	std::function<void()> _func;
};
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
	EmailError = 1003,
	MysqlError = 1004,
	UserExist = 1005,
	UserNotExist = 1006,
	PasswdError = 1007,
	UserLogined = 1008,
};
