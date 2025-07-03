#ifndef CHATDLG_H
#define CHATDLG_H

#include <QDialog>
#include "const.h"
#include "tcpmgr.h"
#include "myinfomgr.h"
namespace Ui {
class ChatDlg;
}

class ChatDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDlg(QWidget *parent = nullptr,LoginInfo info = LoginInfo());
    ~ChatDlg();
signals:
    void sig_connect_tcp(const QString& host,const QString& port);
    void sig_send_data(ReqId id,QByteArray data);
    void sig_return_loginDlg();
public slots:
    void slot_connect_successs(bool);
    void slot_receive_data(ReqId id,QByteArray data);
private:
    void UpdateInterface();
    void initialHandlers();
private:
    std::unordered_map<ReqId,handler> _handlers;
    std::shared_ptr<TcpMgr> _tcpmgr;
    Ui::ChatDlg *ui;
    std::shared_ptr<MyInfoMgr> _myInfoMgr;
    LoginInfo _loginInfo;
};

#endif // CHATDLG_H
