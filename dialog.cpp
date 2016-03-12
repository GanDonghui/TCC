#include "myhelper.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "iconhelper.h"
#include <QSqlDatabase>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include<widget.h>
#include<QSqlError>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
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

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void Dialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void Dialog::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}


void Dialog::set(int i)
{
    ui->stackedWidget->setCurrentIndex(i);
    if(i==0)
        ui->lab_Title->setText("入库");
    else
        ui->lab_Title->setText("出库");
}

double Dialog::total_fee(QTime it,QTime ot)
{
    double total=0;
    int i=1;
    QTime et,st,t(0,0);
    if(ot<it)
    {
        et=t;
    }
    else
        et=ot;
    QSqlQuery l;
    if(et.hour()==0&&et.minute()==0)
        l.exec("select *from sfsz where end_time>\'"+it.toString()+"\' or end_time='0:00' order by start_time");
    else
        l.exec("select *from sfsz where start_time<\'"+et.toString()+"\' and end_time>\'"+it.toString()+"\' order by end_time");
    st=it;
    while(l.next())
    {
        if(i==0)
            st=l.value(0).toTime();
        if(et>=l.value(1).toTime()||et==t)
        {
            double to=l.value(1).toTime().hour()-st.hour()+(double)(l.value(1).toTime().minute()-st.minute())/60;
            if(l.value(1).toTime().hour()==0&&l.value(1).toTime().minute()==0)
            {
                to+=24;
            }
            total+=l.value(2).toDouble()*(to);
        }
        else
        {
           total+=l.value(2).toDouble()*(et.hour()-st.hour()+(double)(et.minute()-st.minute())/60);
        }
        i=0;
    }
    if(ot<it)
    {
        l.exec("select *from sfsz1 where start_time<\'"+ot.toString()+"\' order by start_time");
        while(l.next())
        {
            if(ot<l.value(1).toTime()||(l.value(1).toTime().hour()==0&&l.value(1).toTime().minute()==0))
               total+=l.value(2).toDouble()*(ot.hour()-l.value(0).toTime().hour()+(double)(ot.minute()-l.value(0).toTime().minute())/60);
            else
               total+=l.value(2).toDouble()*(l.value(1).toTime().hour()-l.value(0).toTime().hour()+(double)(l.value(1).toTime().minute()-l.value(0).toTime().minute())/60);
        }
    }
    return total;
}

//车辆入库
void Dialog::on_pushButton_clicked()
{
    QString id=ui->lineEdit_4->text();
    QString plates=ui->lineEdit_6->text();
    QString ms=ui->comboBox_2->currentText();
    if(id.isEmpty()||plates.isEmpty())
    {
      myHelper::ShowMessageBoxError("卡号和车牌号不能为空\n请重新输入！");
      return ;
    }
    if(ms!="vip")
    {
        QSqlQuery q,l;
        Widget w;
        QString cwnum;
        q.exec("select state,park from gdcw where num=\'"+id+"\'");
        if(q.next())
        {
            if(q.value(0).toString()=="正常")
            {
                myHelper::ShowMessageBoxError("此卡号为固定车位卡号且未过期，请选择vip收费方式！");
                return;
            }
        }
        l.exec("select cwnum from cw where state=0;");
        l.next();
        if(l.value(0).isNull())
        {
            myHelper::ShowMessageBoxError("错误！暂时无空余临时车位.");
            return ;
        }
        cwnum=l.value(0).toString();
        l.prepare("update cw set state=1 where cwnum=(:cwnum)");
        l.bindValue(":cwnum",cwnum);
        l.exec();
        q.prepare("INSERT INTO lscw(num,plates,park,in_time,model)"
                                        "VALUES (:num,:plates,:cwnum,now(),:model)");
        q.bindValue(":num",id);
        q.bindValue(":plates",plates);
        q.bindValue(":cwnum",cwnum);
        q.bindValue(":model",ms);
        bool ok = q.exec();
        if(ok)
            myHelper::ShowMessageBoxInfo("入库成功！\n请将车辆停入 "+cwnum+" 车位.");
        else
            myHelper::ShowMessageBoxError("卡号输入错误或该卡号已存在");
        w.update_tableview();
    }
    else
    {
        QSqlQuery q;
        QString cwnum;
        q.exec("select state,online,park from gdcw where num=\'"+id+"\'");
        q.next();
        if(q.value(0).isNull())
        {
            myHelper::ShowMessageBoxError("警告！此卡号不是VIP账户.\n请输入正确卡号或选择其它收费模式.");
            return ;
        }
        else if(q.value(0).toString()=="已过期")
        {
            myHelper::ShowMessageBoxError("警告！此VIP账户已到期.\n请续费或选择其它收费模式.");
            return ;
        }
        else
        {
            if(q.value(1).toString()=="是")
            {
                myHelper::ShowMessageBoxError("警告！此VIP账号正在使用.\n请输入正确卡号.");
                return ;
            }
            else
            {
                cwnum=q.value(2).toString();
                q.exec("update gdcw set online='是',plates=\'"+plates+"\' where num=\'"+id+"\'");
                q.exec("insert into jcjl values(\'"+id+"\',\'"+plates+"\',now(),'入库')");
                Widget w;
                w.update_tableview_2();
                myHelper::ShowMessageBoxInfo("入库成功！\n请将车辆停入"+cwnum+"车位.");
            }
        }
    }
}

//车辆出库
void Dialog::on_pushButton_2_clicked()
{
     QString num=ui->lineEdit->text();
     if(num.isEmpty())
     {
         myHelper::ShowMessageBoxError("请输入卡号！");
         return ;
     }
     QSqlQuery q;
     q.prepare("select in_time,model from lscw where num=(:num)");
     q.bindValue(":num",num);
     q.exec();
     if(q.next())
     {
         QDateTime i=q.value(0).toDateTime();
         QString m=q.value(1).toString();
         QDate id=i.date();
         QTime it=i.time();
         QDateTime o=QDateTime::currentDateTime();
         QDate od=o.date();
         QTime ot=o.time();
         double total;
         int j=0;
         while(od!=id)
         {
             id=id.addDays(1);
             j++;
         }
         if(ot<it||(it.hour()==0&&it.minute()==0))
             j--;
         QTime zero(0,0);
         total=total_fee(it,ot)+j*total_fee(zero,zero);;
         QString total_s;
         total_s=total_s.setNum(total);
         q.prepare("select park from lscw where num=(:num)");
         q.bindValue(":num",num);
         q.exec();
         q.next();
         QString park=q.value(0).toString();
         q.prepare("update cw set state=0 where cwnum=(:cwnum)");
         q.bindValue(":cwnum",park);
         q.exec();
         q.prepare("delete from lscw where num=(:num)");
         q.bindValue(":num",num);
         bool b=q.exec();
         if(!b)
         {
             myHelper::ShowMessageBoxError("出库失败！");
             return ;
         }
         myHelper::ShowMessageBoxInfo("操作成功\n共计收费 "+total_s+"元");
         q.exec("insert into zd values(curdate(),0)");
         q.prepare("update zd set pay=pay+(:pay) where d=(select curdate())");
         q.bindValue(":pay",total);
         b=q.exec();
         if(!b)
         {
            myHelper::ShowMessageBoxError("账单更新出错！");
            return ;
         }
         Widget w;
         w.update_tableview();
         return;
     }
     q.exec("select *from gdcw where num=\'"+num+"\'");
     if(q.next())
     {
         QSqlQuery l;
         if(q.value(4).toString()=="否")
         {
             myHelper::ShowMessageBoxError("卡号输入错误！此卡号账户未入库");
             return;
         }
         l.exec("select plates from gdcw where num=\'"+num+"\'");
         l.next();
         QString plates=l.value(0).toString();
         l.exec("insert into jcjl values(\'"+num+"\',\'"+plates+"\',now(),'出库')");
         l.exec("update gdcw set online='否' where num=\'"+num+"\'");
         QString park=q.value(3).toString();
         if(q.value(6).toString()=="已过期")
             l.exec("update cw set state=0 where cwnum=\'"+park+"\'");
         myHelper::ShowMessageBoxInfo("出库成功！");
         Widget w;
         w.update_tableview_2();
         return ;
     }
     else
     {
         myHelper::ShowMessageBoxError("卡号输入错误，请重新输入！");
         return ;
     }
}
