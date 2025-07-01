#ifndef TCPMGR_H
#define TCPMGR_H
#include<QNetworkAccessManager>
#include<QUrl>
#include"const.h"
#include"singleton.h"
#include<QTcpSocket>
#include<QTcpServer>
class TcpMgr:public Singleton<TcpMgr>
{
    friend class Singleton<TcpMgr>;
public:

private:
    TcpMgr();
private:
    QNetworkAccessManager *_manager;
    QTcpSocket *_socket;
    QTcpServer *_server;
};

#endif // TCPMGR_H
