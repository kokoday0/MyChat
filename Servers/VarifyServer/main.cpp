#include"VarifyGrpcServer.h"
int main()
{
	VarifyGrpcServer::GetInstance()->Start();
	return 0;
}