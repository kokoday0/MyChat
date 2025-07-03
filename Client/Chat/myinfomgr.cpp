#include "myinfomgr.h"
MyInfoMgr::MyInfoMgr(int uid, QString name, QString icon)
    :_uid(uid),_name(name),_icon(icon)
{

}

QString MyInfoMgr::GetMyName()
{
    return _name;
}

QString MyInfoMgr::GetMyIcon()
{
    return _icon;
}

int MyInfoMgr::GetMyUid()
{
    return _uid;
}

bool MyInfoMgr::SetMyName(QString name)
{
    _name = name;
}
