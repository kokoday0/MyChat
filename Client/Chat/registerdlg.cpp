#include "registerdlg.h"
#include "ui_registerdlg.h"
#include "httpmgr.h"

RegisterDlg::RegisterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDlg)
{
    ui->setupUi(this);
    _timer.setSingleShot(true);
    //初始化回调函数
    //连接http发送完成信号和槽
    //点击获取按钮计时10s
    connect(&_timer,&QTimer::timeout,[this](){
        ui->acq_btn->setEnabled(true);
    });
    //初始化regex格式
    _regex_email = QRegularExpression(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    _regex_user = QRegularExpression(R"(\w[5,12])");
    _regex_passwd = QRegularExpression(R"(\w[5,12])");
}

RegisterDlg::~RegisterDlg()
{
    delete ui;
}



void RegisterDlg::buttonCntTime(int seconds)
{
    ui->acq_btn->setEnabled(false);
    _timer.start(seconds*1000);
}


void RegisterDlg::on_acq_btn_clicked()
{
    auto email = ui->email_ed->text();
    bool match = _regex_email.match(email).hasMatch();
    if(!match)
    {
        emit sig_show_message(DialogClass::Error,"邮箱格式不正确！");
        return;
    }
    QJsonObject jsonObj;
    jsonObj["email"] = email;

    HttpMgr::GetInstance()->PostHttpReq(ID_GET_VARIFY_CODE,gate_url_prefix + "get_varifycode",jsonObj);
    buttonCntTime(10);
}

void RegisterDlg::on_cancel_btn_clicked()
{
    emit sig_switch_loginDlg();
}

void RegisterDlg::on_sure_btn_clicked()
{
    //检查格式
    bool user_match = _regex_user.match(ui->username_ed->text()).hasMatch();
    if(!user_match)
    {
        emit sig_show_message(DialogClass::Error,"用户名格式不正确");
        return;
    }
    bool passwd_match = _regex_passwd.match(ui->passwd_ed->text()).hasMatch();
    if(!passwd_match)
    {
        emit sig_show_message(DialogClass::Error,"密码格式不正确");
        return;
    }
    bool email_match = _regex_email.match(ui->email_ed->text()).hasMatch();
    if(!email_match)
    {
        emit sig_show_message(DialogClass::Error,"邮箱格式不正确");
        return;
    }
    //发送http验证验证码是否匹配
    QJsonObject jsonObj;
    jsonObj["user"] = ui->username_ed->text();
    jsonObj["email"] = ui->email_ed->text();
    jsonObj["passwd"] = ui->passwd_ed->text();
    jsonObj["varifycode"] = ui->code_ed->text();
    HttpMgr::GetInstance()->PostHttpReq(ReqId::ID_REG_USER,QUrl(gate_url_prefix + "user_register"),jsonObj);
}
