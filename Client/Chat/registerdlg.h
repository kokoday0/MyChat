#ifndef REGISTERDLG_H
#define REGISTERDLG_H
#include"const.h"
#include <QDialog>
#include <QRegularExpression>
namespace Ui {
class RegisterDlg;
}

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDlg(QWidget *parent = nullptr);
    ~RegisterDlg();
signals:
    void sig_switch_loginDlg();
    void sig_show_message(DialogClass dc,QString str);
private:
    void buttonCntTime(int seconds);
private slots:
    void on_acq_btn_clicked();
    void on_cancel_btn_clicked();
    void on_sure_btn_clicked();
private:
    Ui::RegisterDlg *ui;
    QTimer _timer;
    QRegularExpression _regex_user;
    QRegularExpression _regex_passwd;
    QRegularExpression _regex_email;
};

#endif // REGISTERDLG_H
