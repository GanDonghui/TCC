#include "widget.h"
#include "dialog.h"
#include "login.h"
#include "connect.h"
#include <QApplication>
#include "myhelper.h"
#include <QSqldatabase>
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>
#include <QSqlError>
#include <QByteArray>

QSqlDatabase db;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath())+QDir::separator()+
    "plugins");
    QApplication::addLibraryPath(strLibPath);
    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
    myHelper::SetStyle("blue1");//蓝色风格
    myHelper::SetChinese();
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("tcc");
    db.setUserName("root");
    db.setPassword("1234");
    if (!db.open())
    {
       myHelper::ShowMessageBoxError(""+db.lastError().text()+"");
    }
    else
    {
        Login l;
        l.show();
        return a.exec();
    }
}
