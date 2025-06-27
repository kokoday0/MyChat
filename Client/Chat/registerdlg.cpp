#include "registerdlg.h"
#include "ui_registerdlg.h"
#include "httpmgr.h"
#include <QRegularExpression>
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
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
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
