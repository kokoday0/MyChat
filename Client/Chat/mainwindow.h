#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindlg.h"
#include "registerdlg.h"
#include "chatdlg.h"
#include "const.h"
#include "UserInfo.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void sig_show_message(DialogClass dc,QString str);
    void sig_switch_chatDlg(LoginInfo info);
private:
    void initialHandlers();
private slots:
    void slot_show_message(DialogClass dc,QString str);
    void slot_switch_registerDlg();
    void slot_switch_loginDlg();
    void slot_return_loginDlg();
    void slot_switch_chatDlg(LoginInfo info);
    void slot_http_finished(ReqId id, QByteArray res, ErrorCode err);
private:
    Ui::MainWindow *ui;
    LoginDlg *_loginDlg;
    RegisterDlg *_registerDlg;
    ChatDlg *_chatDlg;
    std::unordered_map<ReqId,handler> _login_handlers;
    std::unordered_map<ReqId,handler> _regis_handlers;
};
#endif // MAINWINDOW_H
