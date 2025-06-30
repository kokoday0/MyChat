#include "logindlg.h"
#include "ui_logindlg.h"
#include "registerdlg.h"
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
