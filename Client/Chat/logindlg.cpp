#include "logindlg.h"
#include "ui_logindlg.h"
#include"httpmgr.h"
LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
}

LoginDlg::~LoginDlg()
{
    delete ui;
}


void LoginDlg::on_register_btn_clicked()
{
    emit sig_switch_registerDlg();
}

void LoginDlg::on_login_btn_clicked()
{
    //用户名和密码是否符合格式
    QString username = ui->username_ed->text();
    QString passwd = ui->passwd_ed->text();
    if(username.length() < 5 || username.length() > 12)
    {
        emit sig_show_message(DialogClass::Error,"用户名格式错误");
        return;
    }
    if(passwd.length() <5 || passwd.length() > 12)
    {
        emit sig_show_message(DialogClass::Error,"密码格式错误");
        return;
    }
    //发送http连接
    QJsonObject jsonObj;
    jsonObj["user"] = username;
    jsonObj["passwd"] = passwd;
    HttpMgr::GetInstance()->PostHttpReq(ReqId::ID_USER_LOGIN,gate_url_prefix + "user_login",jsonObj);
}
