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
    initialHandlers();
    //连接http发送完成信号和槽
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_http_finished,this,&RegisterDlg::slot_http_finished);
    //点击获取按钮计时10s
    connect(&_timer,&QTimer::timeout,[this](){
        ui->acq_btn->setEnabled(true);
    });
    //初始化regex格式
    _regex_email = QRegularExpression(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    _regex_user = QRegularExpression(R"(\w[5,12])");
}

RegisterDlg::~RegisterDlg()
{
    delete ui;
}

void RegisterDlg::initialHandlers()
{
    _handlers[ReqId::ID_GET_VARIFY_CODE] = [this](const QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        auto email = jsonObj["email"].toString();
        if(error!=ErrorCode::SUCCESS)
        {
            showMessage(DialogClass::Error,"网络错误");
        }
        else
        {
            showMessage(DialogClass::Info,"验证码已经发送到邮箱" + email + ",请注意查收");
        }
    };

    _handlers[ReqId::ID_REG_USER] = [this](const QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error == ErrorCode::SUCCESS)
        {
            int uid = jsonObj["uid"].toInt();
            showMessage(DialogClass::Info,"注册成功！");
            on_cancel_btn_clicked();
        }
        else
        {
            showMessage(DialogClass::Error,ErrorInfo[static_cast<ErrorCode>(error)]);
        }
    };
}

void RegisterDlg::showMessage(DialogClass dc, QString str)
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

void RegisterDlg::buttonCntTime(int seconds)
{
    ui->acq_btn->setEnabled(false);
    _timer.start(seconds*1000);
}

void RegisterDlg::slot_http_finished(ReqId id,QByteArray res, ErrorCode err)
{
   if(err!=SUCCESS)
   {
       showMessage(DialogClass::Error,"发送请求失败");
       return;
   }
   QJsonDocument jsonDoc = QJsonDocument::fromJson(res);
   if(jsonDoc.isNull() || !jsonDoc.isObject())
   {
       qDebug() << "json解析错误";
       showMessage(DialogClass::Error,"json解析错误");
       return;
   }
   QJsonObject jsonObj = jsonDoc.object();
   //执行回调
   _handlers[id](jsonObj);
}

void RegisterDlg::on_acq_btn_clicked()
{
    auto email = ui->email_ed->text();
    bool match = _regex_email.match(email).hasMatch();
    if(!match)
    {
        showMessage(DialogClass::Error,"邮箱格式不正确！");
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
        showMessage(DialogClass::Error,"用户名格式不正确");
        return;
    }
    bool email_match = _regex_email.match(ui->email_ed->text()).hasMatch();
    if(!email_match)
    {
        showMessage(DialogClass::Error,"邮箱格式不正确");
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
