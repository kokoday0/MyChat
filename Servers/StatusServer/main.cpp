#include"StatusServer.h"
int main()
{
	StatusServer::GetInstance()->Start();
	return 0;
}