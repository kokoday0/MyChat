#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "httpmgr.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("QQ");
    this->setWindowFlags(this->windowFlags()&~Qt::Tool);
    _loginDlg = new LoginDlg(this);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setFixedSize(_loginDlg->size());
    this->setCentralWidget(_loginDlg);
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_http_finished,this,&MainWindow::slot_http_finished);
    connect(_loginDlg,&LoginDlg::sig_switch_registerDlg,this,&MainWindow::slot_switch_registerDlg);
    connect(_loginDlg,&LoginDlg::sig_show_message,this,&MainWindow::slot_show_message);
    connect(this,&MainWindow::sig_switch_chatDlg,this,&MainWindow::slot_switch_chatDlg);
    connect(this,&MainWindow::sig_show_message,this,&MainWindow::slot_show_message);
    initialHandlers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialHandlers()
{
    _login_handlers[ReqId::ID_USER_LOGIN] = [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCode::SUCCESS)
        {
            emit sig_show_message(DialogClass::Error,ErrorInfo[static_cast<ErrorCode>(error)]);
            return;
        }
        //读取json
        int uid = jsonObj["uid"].toInt();
        QString token = jsonObj["token"].toString();
        QString host = jsonObj["host"].toString();
        QString port = jsonObj["port"].toString();
        qDebug() << "uid is : " << uid<<",host is : " <<host<<",port is : " <<port <<",token is : "<<token;
        emit sig_show_message(DialogClass::Info,"登录成功");
        emit sig_switch_chatDlg(LoginInfo(uid,host,port,token));
    };

    _regis_handlers[ReqId::ID_GET_VARIFY_CODE] = [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        auto email = jsonObj["email"].toString();
        if(error!=ErrorCode::SUCCESS)
        {
            emit sig_show_message(DialogClass::Error,"网络错误");
        }
        else
        {
            emit sig_show_message(DialogClass::Info,"验证码已经发送到邮箱" + email + ",请注意查收");
        }
    };

    _regis_handlers[ReqId::ID_REG_USER] = [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error == ErrorCode::SUCCESS)
        {
            int uid = jsonObj["uid"].toInt();
            emit sig_show_message(DialogClass::Info,"注册成功！");
            slot_switch_loginDlg();
        }
        else
        {
            emit sig_show_message(DialogClass::Error,ErrorInfo[static_cast<ErrorCode>(error)]);
        }
    };
}

void MainWindow::slot_show_message(DialogClass dc, QString str)
{
    switch (dc) {
    case Info:{
        QMessageBox::information(this,"info",str);
        break;
    }
    case Warning:{
        QMessageBox::warning(this,"warning",str);
        break;
    }
    case Error:{
        QMessageBox::critical(this,"error",str);
        break;
    }
    default:
        break;
    }
}


void MainWindow::slot_switch_registerDlg()
{
    _registerDlg = new RegisterDlg(this);
    this->setCentralWidget(_registerDlg);
    this->setFixedSize(_registerDlg->size());
    connect(_registerDlg,&RegisterDlg::sig_switch_loginDlg,this,&MainWindow::slot_switch_loginDlg);
    connect(_registerDlg,&RegisterDlg::sig_show_message,this,&MainWindow::slot_show_message);
    _registerDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _loginDlg->hide();
    _registerDlg->show();
}

void MainWindow::slot_switch_loginDlg()
{
    _loginDlg = new LoginDlg(this);
    this->setCentralWidget(_loginDlg);
    this->setFixedSize(_loginDlg->size());
    connect(_loginDlg,&LoginDlg::sig_switch_registerDlg,this,&MainWindow::slot_switch_registerDlg);
    connect(_loginDlg,&LoginDlg::sig_show_message,this,&MainWindow::slot_show_message);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _registerDlg->hide();
    _loginDlg->show();
}

void MainWindow::slot_return_loginDlg()
{
    _loginDlg = new LoginDlg(this);
    this->setCentralWidget(_loginDlg);
    this->setFixedSize(_loginDlg->size());
    connect(_loginDlg,&LoginDlg::sig_switch_registerDlg,this,&MainWindow::slot_switch_registerDlg);
    connect(_loginDlg,&LoginDlg::sig_show_message,this,&MainWindow::slot_show_message);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _chatDlg->hide();
    _loginDlg->show();
}

void MainWindow::slot_switch_chatDlg(LoginInfo info)
{
    _chatDlg = new ChatDlg(this,info);
    this->setCentralWidget(_chatDlg);
    this->setFixedSize(_chatDlg->size());
    connect(_chatDlg,&ChatDlg::sig_return_loginDlg,this,&MainWindow::slot_return_loginDlg);
    _chatDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _loginDlg->hide();
    _chatDlg->show();
}

void MainWindow::slot_http_finished(ReqId id, QByteArray res, ErrorCode err)
{
    if(err!=SUCCESS)
    {
        emit sig_show_message(DialogClass::Error,"发送请求失败");
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res);
    if(jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "json解析错误";
        emit sig_show_message(DialogClass::Error,"json解析错误");
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    //执行回调
    if(id==ReqId::ID_USER_LOGIN)
        _login_handlers[id](jsonObj);
    else
        _regis_handlers[id](jsonObj);
}



