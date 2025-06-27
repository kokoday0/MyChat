#include "mainwindow.h"

#include <QApplication>
#include"const.h"
#include<QSettings>
#include<QDir>
QString gate_url_prefix = "";
int main(int argc, char *argv[])
{

    //需要先实例化QCoreApplication
    QApplication app(argc, argv);
    //读取配置 pro中将配置文件复制到exe目录下
    QString cur_path = QCoreApplication::applicationDirPath();
    QString filename = "config.ini";
    QString file_path = QDir::toNativeSeparators(cur_path + QDir::separator() + filename);
    QSettings settings(file_path,QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = QString("http://"+gate_host+":"+gate_port+"/");
    //
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
