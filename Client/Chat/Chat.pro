QT       += core gui
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    httpmgr.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdlg.cpp \
    singleton.cpp \
    tcpmgr.cpp

HEADERS += \
    const.h \
    httpmgr.h \
    mainwindow.h \
    registerdlg.h \
    singleton.h \
    tcpmgr.h

FORMS += \
    mainwindow.ui \
    registerdlg.ui

CONFIG(debug, debug|release) {
targetDir = $$OUT_PWD/Debug/config.ini
} else {
targetDir = $$OUT_PWD/Release/config.ini
}

# 在构建时将 config.ini 文件拷贝到目标目录
win32 {
CONFIG_FILE = $$PWD/config.ini
CONFIG_FILE = $$replace(CONFIG_FILE, /, \\)
targetDir = $$replace(targetDir, /, \\)
QMAKE_POST_LINK += copy /Y \"$$CONFIG_FILE" \"$$targetDir"
}
