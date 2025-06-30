#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>

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
private slots:
    void on_register_btn_clicked();
private:
    Ui::LoginDlg *ui;
};

#endif // LOGINDLG_H
