#ifndef HTTPMGR_H
#define HTTPMGR_H
#include"const.h"
#include<QUrl>
#include"singleton.h"
class HttpMgr : public QObject,public Singleton<HttpMgr>,public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
    friend class Singleton<HttpMgr>;
public:
    void PostHttpReq(ReqId id,QUrl url,QJsonObject json);
signals:
    void sig_http_finished(ReqId id,QByteArray res,ErrorCode err);
private:
    HttpMgr();
private:
    QNetworkAccessManager _network_manager;
};

#endif // HTTPMGR_H
