#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include"const.h"
namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();
signals:
    void sig_switch_registerDlg();
    void sig_show_message(DialogClass dc,QString str);
private slots:
    void on_register_btn_clicked();
    void on_login_btn_clicked();
private:
    Ui::LoginDlg *ui;
    std::unordered_map<ReqId,handler> _handlers;
};

#endif // LOGINDLG_H
