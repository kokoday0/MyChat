#ifndef USERINFO_H
#define USERINFO_H
#include <QString>
struct LoginInfo{
    int _uid;
    QString _host;
    QString _port;
    QString _token;
    LoginInfo(){}
    LoginInfo(int uid,QString host,QString port,QString token):_uid(uid),_host(host),_port(port),_token(token){}
};
#endif // USERINFO_H
