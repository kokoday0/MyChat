#ifndef CONST_H
#define CONST_H
#include<QString>
#include<QObject>
#include<QMap>
#include<QTimer>
#include<QMessageBox>
#include<memory>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkAccessManager>

enum ReqId{
    ID_GET_VARIFY_CODE = 1001,
    ID_REG_USER = 1002,
};

enum ErrorCode{
    SUCCESS = 0,
    ERR_JSON = 1001,
};
enum DialogClass{
    Info = 0,
    Warning = 1,
    Error = 2,
};

extern QString gate_url_prefix;
#endif // CONST_H
