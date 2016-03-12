#include "login.h"
#include "ui_login.h"
#include "iconhelper.h"
#include "myhelper.h"
#include "widget.h"
#include <QSqlQuery>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->lineEdit->setEchoMode(QLineEdit::Password);
    this->mousePressed = false;
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置图形字体
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 14);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 12);
    //关联关闭按钮
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    //窗体居中显示
    myHelper::FormInCenter(this);
}

Login::~Login()
{
    delete ui;
}

void Login::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void Login::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void Login::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void Login::on_pushButton_2_clicked()
{
    close();
}

void Login::on_pushButton_clicked()
{
    QString p=ui->lineEdit->text();
    QString pmd5;
    QByteArray b,ba;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(p);
    md.addData(ba);
    b = md.result();
    pmd5.append((b.toHex()));
    QSqlQuery q;
    q.exec("select *from passward");
    q.next();
    QString s=q.value(0).toString();
    if(pmd5!=s)
    do
    {
        if(i==2)
        {
            myHelper::ShowMessageBoxError("系统检测您已连续三次输错密码，请稍后再试！");
            qApp->quit();
        }
        myHelper::ShowMessageBoxError("密码错误！请重新输入");
        p=ui->lineEdit->text();
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
        i++;
    }while(p!=s&&ui->pushButton->isChecked());
    else
    {
        close();
        w=new Widget;
        w->update_view();
        w->show();
    }


}
