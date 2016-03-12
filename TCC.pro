#-------------------------------------------------
#
# Project created by QtCreator 2015-06-07T10:57:43
#
#-------------------------------------------------

QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TCC
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    dialog.cpp \
    frmmessagebox.cpp \
    iconhelper.cpp \
    login.cpp

HEADERS  += widget.h \
    dialog.h \
    myhelper.h \
    frmmessagebox.h \
    iconhelper.h \
    login.h \
    connect.h

FORMS    += widget.ui \
    dialog.ui \
    frmmessagebox.ui \
    login.ui

RESOURCES += \
    rc.qrc

win32:RC_FILE=your.rc


