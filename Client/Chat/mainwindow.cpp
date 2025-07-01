#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "httpmgr.h"

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
            emit slot_show_message(DialogClass::Error,ErrorInfo[static_cast<ErrorCode>(error)]);
            return;
        }
        //读取json
        int uid = jsonObj["uid"].toInt();
        QString token = jsonObj["token"].toString();
        QString host = jsonObj["host"].toString();
        QString port = jsonObj["port"].toString();
        qDebug() << "uid is : " << uid<<",host is : " <<host<<",port is : " <<port <<",token is : "<<token;
        slot_show_message(DialogClass::Info,"登录成功");
        //转到chat窗口
        //建立tcp长连接
    };

    _regis_handlers[ReqId::ID_GET_VARIFY_CODE] = [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        auto email = jsonObj["email"].toString();
        if(error!=ErrorCode::SUCCESS)
        {
            slot_show_message(DialogClass::Error,"网络错误");
        }
        else
        {
            slot_show_message(DialogClass::Info,"验证码已经发送到邮箱" + email + ",请注意查收");
        }
    };

    _regis_handlers[ReqId::ID_REG_USER] = [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error == ErrorCode::SUCCESS)
        {
            int uid = jsonObj["uid"].toInt();
            slot_show_message(DialogClass::Info,"注册成功！");
            slot_switch_loginDlg();
        }
        else
        {
            slot_show_message(DialogClass::Error,ErrorInfo[static_cast<ErrorCode>(error)]);
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

void MainWindow::slot_http_finished(ReqId id, QByteArray res, ErrorCode err)
{
    if(err!=SUCCESS)
    {
        slot_show_message(DialogClass::Error,"发送请求失败");
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res);
    if(jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "json解析错误";
        slot_show_message(DialogClass::Error,"json解析错误");
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    //执行回调
    if(id==ReqId::ID_USER_LOGIN)
        _login_handlers[id](jsonObj);
    else
        _regis_handlers[id](jsonObj);
}



