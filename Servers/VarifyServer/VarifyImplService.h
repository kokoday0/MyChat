#pragma once
#include"const.h"
class VarifyImplService final:public message::VarifyService::Service
{
public:
	VarifyImplService();
	grpc::Status GetVarifyCode(grpc::ServerContext* context,const message::GetVarifyReq* req, message::GetVarifyRsp* rsp) override;\
private:
	bool SendEmail(const std::string& email,const std::string& code);
	std::string GetRandomCode(int num);
};

