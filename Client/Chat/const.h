#ifndef CONST_H
#define CONST_H
#include<QString>
#include<QObject>
#include<QMap>
#include<unordered_map>
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
    ID_USER_LOGIN = 1003,
};
enum RspId {
    ID_LOGIN_RSP = 0,
};

enum ErrorCode{
    SUCCESS = 0,
    ERR_JSON = 1001,
    RPCFailed = 1002,
    EmailError = 1003,
    MysqlError = 1004,
    UserExist = 1005,
    UserNotExist = 1006,
    PasswdError  = 1007,
    UserLogined = 1008,
};
enum DialogClass{
    Info = 0,
    Warning = 1,
    Error = 2,
};
typedef std::function<void(const QJsonObject jsonObj)> handler;
extern QString gate_url_prefix;
extern std::unordered_map<ErrorCode,QString> ErrorInfo;
#endif // CONST_H
