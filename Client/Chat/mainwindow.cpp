#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("QQ");
    this->setWindowFlags(this->windowFlags()&~Qt::Tool);
    _loginDlg = new LoginDlg(this);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setFixedSize(_loginDlg->size());
    this->setCentralWidget(_loginDlg);
    connect(_loginDlg,&LoginDlg::sig_switch_registerDlg,this,&MainWindow::slot_switch_registerDlg);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_switch_registerDlg()
{
    _registerDlg = new RegisterDlg(this);
    this->setCentralWidget(_registerDlg);
    this->setFixedSize(_registerDlg->size());
    connect(_registerDlg,&RegisterDlg::sig_switch_loginDlg,this,&MainWindow::slot_switch_loginDlg);
    _registerDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _loginDlg->hide();
    _registerDlg->show();
}

void MainWindow::slot_switch_loginDlg()
{
    _loginDlg = new LoginDlg(this);
    this->setCentralWidget(_loginDlg);
    this->setFixedSize(_loginDlg->size());
    connect(_loginDlg,&LoginDlg::sig_switch_registerDlg,this,&MainWindow::slot_switch_registerDlg);
    _loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _registerDlg->hide();
    _loginDlg->show();
}


