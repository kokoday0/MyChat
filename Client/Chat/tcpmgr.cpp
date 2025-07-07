#include "tcpmgr.h"

void TcpMgr::slot_connect_tcp(const QString &host, const QString &port)
{
    qDebug() << "Connect to ChatServer...";
    _port = port.toUInt();
    _host = host;
    _socket.connectToHost(host,_port);
}

void TcpMgr::slot_send_data(ReqId id,QByteArray data)
{
    quint16 datalen = static_cast<quint16>(data.size());
    QByteArray block;
    QDataStream datastream(&block,QIODevice::WriteOnly);
    datastream.setByteOrder(QDataStream::BigEndian);
    datastream << static_cast<quint16>(id) << datalen;
    block.append(data);
    _socket.write(block);
}

TcpMgr::TcpMgr():_b_receive_head(true)
{
    connect(&_socket,&QTcpSocket::connected,[this](){
        qDebug() << "TCP连接成功";
        emit sig_connect_success(true);
    });
    //接收数据
    connect(&_socket,&QTcpSocket::readyRead,[&](){
        qDebug() << "Ready to Receive Data";
        _buffer.append(_socket.readAll());
        QDataStream datastream(&_buffer,QIODevice::ReadOnly);
        forever{
            //检查缓冲区是否足够解析出头节点
            if(_b_receive_head)
            {
                if(_buffer.size() < static_cast<int>(sizeof (quint16)*2))
                    return;
                datastream >> _message_id >> _message_len;
                _buffer = _buffer.mid(sizeof(quint16)*2);
                qDebug() << "Receive ReqId is : "<<_message_id << " length is : " <<_message_len;
            }
            if(_buffer.size() < _message_len)
            {
                _b_receive_head = false;
                return;
            }
            _b_receive_head = true;
            QByteArray data = _buffer.mid(_message_len);
            _buffer = _buffer.mid(_message_len);
            emit sig_receive_data(static_cast<RspId>(_message_id),data);
        }
    });
    //处理TCP错误
    connect(&_socket,static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),[&](QAbstractSocket::SocketError error){
        emit sig_connect_success(false);
        switch (error) {
        case QTcpSocket::SocketError::HostNotFoundError :{
            qDebug() << "HostNotFoundError";
            break;
        }
        case QTcpSocket::SocketError::NetworkError :{
            qDebug() << "NetworkError";
            break;
        }
        case QTcpSocket::SocketError::SocketTimeoutError :{
            qDebug() << "SocketTimeoutError";
            break;
        }
        default:
            break;
        }
    });
}
