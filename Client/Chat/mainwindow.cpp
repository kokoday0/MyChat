#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "registerdlg.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_register_btn_clicked()
{
    this->hide();
    auto* registerDlg = new RegisterDlg(this);
    registerDlg->show();
}
