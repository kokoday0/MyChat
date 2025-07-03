#include "chatdlg.h"
#include "ui_chatdlg.h"
#include "myinfomgr.h"
ChatDlg::ChatDlg(QWidget *parent,LoginInfo info) :
    QDialog(parent),
    ui(new Ui::ChatDlg)
{
    ui->setupUi(this);
    _loginInfo = info;
    _tcpmgr = TcpMgr::GetInstance();
    connect(this,&ChatDlg::sig_connect_tcp,_tcpmgr.get(),&TcpMgr::slot_connect_tcp);
    connect(this,&ChatDlg::sig_send_data,_tcpmgr.get(),&TcpMgr::slot_send_data);
    connect(_tcpmgr.get(),&TcpMgr::sig_receive_data,this,&ChatDlg::slot_receive_data);
    connect(_tcpmgr.get(),&TcpMgr::sig_connect_success,this,&ChatDlg::slot_connect_successs);
    initialHandlers();
    emit sig_connect_tcp(info._host,info._port);
}

ChatDlg::~ChatDlg()
{
    delete ui;
}

void ChatDlg::slot_connect_successs(bool success)
{
    if(!success)
    {
        emit sig_return_loginDlg();
    }
    QJsonObject jsonObj;
    jsonObj["uid"] = _loginInfo._uid;
    jsonObj["token"] = _loginInfo._token;
    QJsonDocument jsonDoc(jsonObj);
    QByteArray data = jsonDoc.toJson();
    emit sig_send_data(ReqId::ID_USER_LOGIN,data);
}

void ChatDlg::slot_receive_data(ReqId id, QByteArray data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if(jsonDoc.isNull() || !jsonDoc.isObject())
    {
        emit sig_return_loginDlg();
        qDebug() << "Get ChatServer Info Failed";
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    if(jsonObj["error"]!=ErrorCode::SUCCESS)
    {
        qDebug() << "Json Error ";
        return;
    }
    _handlers[id](jsonObj);
}

void ChatDlg::UpdateInterface()
{

}

void ChatDlg::initialHandlers()
{
    _handlers[ReqId::ID_USER_LOGIN] = [this](const QJsonObject& jsonObj){
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto icon = jsonObj["icon"].toString();
        _myInfoMgr = std::make_shared<MyInfoMgr>(uid,name,icon);
        UpdateInterface();
    };
}
