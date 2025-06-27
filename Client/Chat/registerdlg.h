#ifndef REGISTERDLG_H
#define REGISTERDLG_H
#include"const.h"
#include <QDialog>

namespace Ui {
class RegisterDlg;
}
typedef std::function<void(QJsonObject jsonObj)> handler;
class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDlg(QWidget *parent = nullptr);
    ~RegisterDlg();
private:
    void initialHandlers();
    void showMessage(DialogClass dc,QString str);
    void buttonCntTime(int seconds);
private slots:
    void slot_http_finished(ReqId id,QByteArray res,ErrorCode err);
    void on_acq_btn_clicked();
private:
    Ui::RegisterDlg *ui;
    QMap<ReqId,handler> _handlers;
    QTimer _timer;
};

#endif // REGISTERDLG_H
