#ifndef TCPMGR_H
#define TCPMGR_H
#include<QNetworkAccessManager>
#include<QUrl>
#include"const.h"
#include"singleton.h"
#include<QTcpSocket>
#include<QTcpServer>
class TcpMgr:public QObject,public Singleton<TcpMgr>,public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;
signals:
    void sig_connect_success(bool);
    void sig_receive_data(ReqId id,QByteArray data);
public slots:
    void slot_connect_tcp(const QString& host,const QString& port);
    void slot_send_data(ReqId id,QByteArray data);
private:
    TcpMgr();
private:
    QByteArray _buffer;
    QString _host;
    quint16 _port;
    QNetworkAccessManager *_manager;
    QTcpSocket _socket;
    quint16 _message_id;
    quint16 _message_len;
    bool _b_receive_head;
};

#endif // TCPMGR_H
