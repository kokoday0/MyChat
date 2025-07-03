#ifndef MYINFOMGR_H
#define MYINFOMGR_H
#include<QString>
#include "UserInfo.h"
class MyInfoMgr
{
public:
    MyInfoMgr(int uid,QString name,QString icon);
    QString GetMyName();
    QString GetMyIcon();
    int GetMyUid();
    bool SetMyName(QString name);
    bool SetMyIcon(QString icon);
private:
    int _uid;
    QString _name;
    QString _icon;
};

#endif // MYINFOMGR_H
