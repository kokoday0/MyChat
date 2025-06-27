#include "VarifyGrpcClient.h"
#include"CfgMgr.h"
GetVarifyRsp VarifyGrpcClient::GetVarifyCode(std::string email)
{
    ClientContext context;
    GetVarifyReq request;
    GetVarifyRsp response;
    request.set_email(email);
    auto status = _stub->GetVarifyCode(&context, request, &response);
    if (!status.ok())
    {
        std::cout << "VarifyGrpcClient Get Code Fail" << std::endl;
        response.set_error(ErrorCodes::RPCFailed);
    }
    return response;
}

VarifyGrpcClient::VarifyGrpcClient()
{
    CfgMgr& cfg = CfgMgr::Inst();
    std::string target(cfg["VarifyGrpcServer"]["host"] + ":" + cfg["VarifyGrpcServer"]["port"]);
    std::shared_ptr<Channel> channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    _stub = VarifyService::NewStub(channel);
}

