#ifndef TCPMGR_H
#define TCPMGR_H
#include<QNetworkAccessManager>
#include<QUrl>
#include"const.h"
#include"singleton.h"
class TcpMgr:public Singleton<TcpMgr>
{
    friend class Singleton<TcpMgr>;
private:
    TcpMgr();
};

#endif // TCPMGR_H
