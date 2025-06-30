#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindlg.h"
#include "registerdlg.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void slot_switch_registerDlg();
    void slot_switch_loginDlg();

private:
    Ui::MainWindow *ui;
    LoginDlg *_loginDlg;
    RegisterDlg *_registerDlg;
};
#endif // MAINWINDOW_H
