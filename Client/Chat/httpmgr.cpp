//发送Http请求，将reply信息返回给RegisterDlg界面进行处理，post是异步进行，需要保证执行replyfinished前不被析构
#include "httpmgr.h"
#include<QNetworkReply>
#include<QNetworkRequest>
void HttpMgr::PostHttpReq(ReqId id, QUrl url, QJsonObject json)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");//告知服务器请求体数据类型是json
    req.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));//number将data长度转为字节组

    auto self = shared_from_this();
    QNetworkReply* reply = _network_manager.post(req,data);

    QObject::connect(reply,&QNetworkReply::finished,[reply,self,id](){
        if(!reply || reply->error() != QNetworkReply::NoError)
        {
            emit self->sig_http_finished(id,"",ERR_JSON);
            reply->deleteLater();
            return;
        }
        reply->deleteLater();
        QByteArray res = reply->readAll();
        emit self->sig_http_finished(id,res,SUCCESS);
    });
}

HttpMgr::HttpMgr()
{

}
