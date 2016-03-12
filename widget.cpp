#include "widget.h"
#include "ui_widget.h"
#include "connect.h"
#include "myhelper.h"
#include "dialog.h"
#include "iconhelper.h"
#include "QSqlQueryModel"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEdit_16->setEchoMode(QLineEdit::Password);
    ui->lineEdit_17->setEchoMode(QLineEdit::Password);
    ui->lineEdit_18->setEchoMode(QLineEdit::Password);
    QRegExp regx("[.0-9]+$");
    QRegExp r("[1-9][0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    QValidator *validator1 = new QRegExpValidator(r, this );
    ui->lineEdit->setValidator(validator);
    ui->lineEdit_15->setValidator(validator);
    ui->lineEdit_12->setValidator(validator);
    ui->lineEdit_13->setValidator(validator);
    ui->lineEdit_8->setValidator(validator1);
    ui->lineEdit_14->setValidator(validator1);
    ui->lineEdit_15->setValidator(validator1);
    timer = new QTimer(this);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(update_lcd()));
    timer->start(1000);

    myHelper::FormInCenter(this);
    this->InitStyle();
    QFont f("新宋体",14,75);
    ui->pushButton_lscw1->setFont(f);
    ui->pushButton_lscw2->setFont(f);
    ui->pushButton_lscw3->setFont(f);
    ui->pushButton_gdcw1->setFont(f);
    ui->gdcw2->setFont(f);
    ui->pushButton_2->setFont(f);
    ui->pushButton_3->setFont(f);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void Widget::on_btnMenu_Close_clicked()
{
    qApp->exit();
}

void Widget::on_btnMenu_clicked()
{
    myHelper::SetStyle("gray");
    QFont f("新宋体",14,75);
    ui->pushButton_lscw1->setFont(f);
    ui->pushButton_lscw2->setFont(f);
    ui->pushButton_lscw3->setFont(f);
    ui->pushButton_gdcw1->setFont(f);
    ui->gdcw2->setFont(f);
    ui->pushButton_2->setFont(f);
    ui->pushButton_3->setFont(f);
}

void Widget::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void Widget::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void Widget::on_pushButtoni_in_clicked()
{
    d.set(0);
    d.exec();
}

void Widget::on_pushButton_out_clicked()
{
    d.set(1);
    d.exec();
}

//更新临时车库视图
void Widget::update_tableview()
{
    static QSqlQueryModel *mode  = new QSqlQueryModel();   //建立一个Model
    mode->setQuery(QString("select * from lscw")) ;
    mode->setHeaderData(0,Qt::Horizontal,QObject::tr("卡号"));
    mode->setHeaderData(1,Qt::Horizontal,QObject::tr("车牌号"));
    mode->setHeaderData(2,Qt::Horizontal,QObject::tr("车位"));
    mode->setHeaderData(3,Qt::Horizontal,QObject::tr("入库时间"));
    mode->setHeaderData(4,Qt::Horizontal,QObject::tr("收费模式"));
    ui->tableView->setModel(mode);
    ui->tableView->resizeColumnsToContents();//自动调整大小
}

//更新固定车位视图
void Widget::update_tableview_2()
{
    static QSqlQueryModel *mode  = new QSqlQueryModel();   //建立一个Model
    mode->setQuery(QString("select * from gdcw")) ;
    mode->setHeaderData(0,Qt::Horizontal,QObject::tr("卡号"));
    mode->setHeaderData(1,Qt::Horizontal,QObject::tr("姓名"));
    mode->setHeaderData(2,Qt::Horizontal,QObject::tr("车牌号"));
    mode->setHeaderData(3,Qt::Horizontal,QObject::tr("车位"));
    mode->setHeaderData(4,Qt::Horizontal,QObject::tr("是否在库"));
    mode->setHeaderData(5,Qt::Horizontal,QObject::tr("到期日期"));
    mode->setHeaderData(6,Qt::Horizontal,QObject::tr("状态"));
    ui->tableView_2->setModel(mode);
    ui->tableView_2->resizeColumnsToContents();//自动调整大小
}

//更新收费设置视图
void Widget::update_tableview_5()
{
    static QSqlQueryModel *mode  = new QSqlQueryModel();   //建立一个Model
    mode->setQuery(QString("select * from sfsz order by start_time")) ;
    mode->setHeaderData(0,Qt::Horizontal,QObject::tr("开始时间"));
    mode->setHeaderData(1,Qt::Horizontal,QObject::tr("结束时间"));
    mode->setHeaderData(2,Qt::Horizontal,QObject::tr("每小时费用(元)"));
    ui->tableView_5->setModel(mode);
    ui->tableView_5->resizeColumnsToContents();//自动调整大小
}

void Widget::on_pushButton_lscw1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    update_tableview_5();
}

void Widget::on_pushButton_lscw2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    update_tableview_5();
}
void Widget::on_pushButton_lscw3_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

//更新收费设置
bool Widget::update_sfsz(QTime st,QTime et,double f)
{
    QSqlQuery q;
    if(et.hour()==0&&et.minute()==0)
    {
        q.exec("update sfsz set end_time=\'"+st.toString()+"\' where start_time<=\'"+st.toString()+"\' and (end_time>=\'"+st.toString()+"\' or end_time='0:00')");
        q.exec("delete from sfsz where start_time>=\'"+st.toString()+"\'");
        q.prepare("insert into sfsz values(\'"+st.toString()+"\',\'"+et.toString()+"\',:f)");
        q.bindValue(":f",f);
        q.exec();
        return 1;
    }
    else if(st.hour()==0&&st.minute()==0)
    {
        q.exec("update sfsz set start_time=\'"+et.toString()+"\' where start_time<=\'"+et.toString()+"\' and (end_time>=\'"+et.toString()+"\' or end_time='0:00')");
        q.exec("delete from sfsz where end_time<=\'"+et.toString()+"\' and start_time<=\'"+et.toString()+"\'");
        q.prepare("insert into sfsz values(\'"+st.toString()+"\',\'"+et.toString()+"\',:f)");
        q.bindValue(":f",f);
        q.exec();
        return 1;
    }
    else
    {
        q.exec("select *from sfsz where start_time<=\'"+st.toString()+"\' and end_time>=\'"+et.toString()+"\'");
        if(!q.next())
        {
            q.exec("update sfsz set end_time=\'"+st.toString()+"\' where start_time<=\'"+st.toString()+"\' and (end_time>=\'"+st.toString()+"\' or end_time='0:00')");
            q.exec("update sfsz set start_time=\'"+et.toString()+"\' where start_time<=\'"+et.toString()+"\' and (end_time>=\'"+et.toString()+"\' or end_time='0:00')");
            q.exec("delete from sfsz where start_time>=\'"+st.toString()+"\' and end_time<=\'"+et.toString()+"\' and end_time<>'0:00'");
            q.prepare("insert into sfsz values(\'"+st.toString()+"\',\'"+et.toString()+"\',:f)");
            q.bindValue(":f",f);
            q.exec();
            return 1;
        }
        else
        {
            QTime st1=q.value(0).toTime();
            QTime et1=q.value(1).toTime();
            double f1=q.value(2).toDouble();
            q.exec("delete from sfsz where start_time=\'"+st1.toString()+"\'");
            if(st1!=st)
            {
                q.prepare("insert into sfsz values(\'"+st1.toString()+"\',\'"+st.toString()+"\',:f1)");
                q.bindValue(":f1",f1);
                q.exec();
            }
            if(et1!=et)
            {
                q.prepare("insert into sfsz values(\'"+et.toString()+"\',\'"+et1.toString()+"\',:f1)");
                q.bindValue(":f1",f1);
                q.exec();
            }
            q.prepare("insert into sfsz values(\'"+st.toString()+"\',\'"+et.toString()+"\',:f)");
            q.bindValue(":f",f);
            q.exec();
            return 1;
        }
    }
    return 0;
}

//设置收费设置
void Widget::on_pushButton_clicked()
{
    QTime st=ui->timeEdit->time();
    QTime et=ui->timeEdit_2->time();
    QTime zero(0,0);
    QString f=ui->lineEdit->text();
    if(f.isEmpty())
    {
        myHelper::ShowMessageBoxError("警告！每小时收费金额不能为空");
        return;
    }
    double fee=f.toDouble();
    bool ok=0;
    if(st==et)
    {
        myHelper::ShowMessageBoxError("警告！开始时间和结束时间不能相等");
        return ;
    }
    if(st<et||et==zero)
    {
        ok=update_sfsz(st,et,fee);
    }
    else
    {
        ok=update_sfsz(st,zero,fee);
        ok=update_sfsz(zero,et,fee);
    }
    QSqlQuery q;
    q.exec("select *from sfsz order by start_time");
    if(!q.next())
        return;
    QTime s=q.value(0).toTime();
    QTime e=q.value(1).toTime();
    double fe=q.value(2).toDouble();
    while(q.next())
    {
        if(q.value(2).toDouble()==fe&&(q.value(1).toTime()!=zero||q.size()>2))
        {
            QSqlQuery l;
            l.prepare("update sfsz set start_time=(:s) where start_time=\'"+q.value(0).toString()+"\'");
            l.bindValue(":s",s);
            l.exec();
            l.exec("delete from sfsz where end_time=\'"+e.toString()+"\'");
            q.exec("select *from sfsz order by start_time");
            q.next();
        }
        s=q.value(0).toTime();
        e=q.value(1).toTime();
        fe=q.value(2).toDouble();
    }
    update_tableview_5();
    if(ok)
        myHelper::ShowMessageBoxInfo("数据更新成功！");
    else
        myHelper::ShowMessageBoxError("数据跟新出错！");
}

void Widget::on_pushButton_gdcw1_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_gdcw2_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void Widget::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void Widget::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

//固定车位注册
void Widget::on_pushButton_5_clicked()
{
    QSqlQuery l;
    l.exec("select cwnum from cw where state=0");
    l.next();
    if(l.value(0).isNull())
    {
        myHelper::ShowMessageBoxError("对不起，暂时无空余车位.");
        return ;
    }
    QString num=ui->lineEdit_4->text();
    QString name=ui->lineEdit_5->text();
    QString time=ui->lineEdit_8->text();
    QString pay=ui->lineEdit_12->text();
    if(num.isEmpty()||name.isEmpty()||pay.isEmpty()||time.isEmpty())
    {
        myHelper::ShowMessageBoxError("警告！请将信息填满.");
        return ;
    }
    int t=time.toInt();
    double p=pay.toDouble();
    QDate d=QDate::currentDate();
    d=d.addMonths(t);
    QSqlQuery q;
    q.exec("select *from gdcw where num=\'"+num+"\'");
    q.next();
    if(!q.value(0).isNull())
    {
        myHelper::ShowMessageBoxError("警告！此卡号已注册，请更换卡号.");
        return ;
    }
    QString cwnum=l.value(0).toString();
    q.prepare("insert into gdcw values(:num,:name,null,:cwnum,'否',:d,'正常')");
    q.bindValue(":num",num);
    q.bindValue(":name",name);
    q.bindValue(":cwnum",cwnum);
    q.bindValue(":d",d);
    bool ok=q.exec();
    if(!ok)
    {
        myHelper::ShowMessageBoxError("出现未知注册错误！");
        return ;
    }
    else
    {
        q.prepare("update cw set state=1 where cwnum=\'"+cwnum+"\'");
        q.exec();
        l.prepare("insert into jfjl values(:num,now(),:p,:t)");
        l.bindValue(":num",num);
        l.bindValue(":p",p);
        l.bindValue(":t",t);
        l.exec();
        myHelper::ShowMessageBoxInfo("注册成功！此卡号对应车位为"+cwnum+"");
    }
    update_tableview_2();
}

//月帐单查询
void Widget::on_pushButton_7_clicked()
{
    QString year=ui->lineEdit_15->text();
    if(year.isEmpty())
    {
        myHelper::ShowMessageBoxError("警告！请输入年份");
        return;
    }
    QString month=ui->comboBox->currentText();
    static QSqlQueryModel *mode  = new QSqlQueryModel();   //建立一个Model
    mode->setQuery("select * from zd where year(d)=\'"+year+"\' and month(d)=\'"+month+"\'");
    QSqlQuery q=mode->query();
    if(!q.next())
    {
        myHelper::ShowMessageBoxError("未找到此月账单记录！\n请输入其它年和月试试!");
        return;
    }
    double total=q.value(1).toDouble();
    while(q.next())
        total+=q.value(1).toDouble();
    mode->setHeaderData(0,Qt::Horizontal,QObject::tr("日期"));
    mode->setHeaderData(1,Qt::Horizontal,QObject::tr("共收费(元)"));
    ui->tableView_4->setModel(mode);
    ui->tableView_4->resizeColumnsToContents();
    QPalette pe;
    pe.setColor(QPalette::WindowText,"#1B89CA");
    ui->label_9->setPalette(pe);
    QString t;
    t=t.setNum(total);
    t=t+"元";
    ui->label_9->setText(t);
    ui->label_9->setWordWrap(1);
    ui->label_9->setAlignment(Qt::AlignRight);
}

//实时更新空车位数并检查是否连接了数据库
void Widget::update_lcd()
{
   if(!db.commit())
    {
        db.setHostName("localhost");
        db.setDatabaseName("tcc");
        db.setUserName("root");
        db.setPassword("1234");
        if(!db.open())
        {
            myHelper::ShowMessageBoxError(db.lastError().text());
            return;
        }
    }
    QSqlQuery q;
    q.exec("select count(cwnum) from cw where state=0;");
    if(!q.next())
    {
        myHelper::ShowMessageBoxError("警告！数据库无车位数据");
        return;
    }
    QString s=q.value(0).toString();
    QPalette pe;
    pe.setColor(QPalette::WindowText,"#1B89CA");
    ui->label_21->setPalette(pe);
    ui->label_21->setText(s);
    q.exec("select *from cd");
    if(!q.next())
    {
        QSqlQuery l;
        l.exec("insert into cd values(curdate())");
        return;
    }
    if(q.value(0).toDate()!=QDate::currentDate())
    {
        q.exec("update cd set d=curdate()");
        q.exec("select end_time,num,online,park from gdcw where state='正常'");
        while(q.next())
        {
            QDate d=QDate::currentDate();
            if(d>q.value(0).toDate())
            {
                QString num=q.value(1).toString();
                QSqlQuery l;
                l.exec("update gdcw set state='已过期' where num=\'"+num+"\'");
                QString s=q.value(2).toString();
                QString park=q.value(3).toString();
                if(s=="否")
                    l.exec("update cw set state=0 where cwnum=\'"+park+"\'");

            }
        }
        update_view();
    }
}

void Widget::update_view()
{
    update_tableview();
    update_tableview_2();
    update_lcd();
}

//查询固定车位入出库或缴费记录
void Widget::on_pushButton_4_clicked()
{
    QString num=ui->lineEdit_10->text();
    if(num.isEmpty())
    {
        myHelper::ShowMessageBoxError("请输入卡号！");
        return;
    }
    int i=ui->comboBox_2->currentIndex();
    static QSqlQueryModel *mode  = new QSqlQueryModel();   //建立一个Model
    if(i==0)
    {
        mode->setQuery(QString("select *from jcjl where num=\'"+num+"\' order by dt desc")) ;
        QSqlQuery q=mode->query();
        mode->setHeaderData(0,Qt::Horizontal,QObject::tr("卡号"));
        mode->setHeaderData(1,Qt::Horizontal,QObject::tr("车牌号"));
        mode->setHeaderData(2,Qt::Horizontal,QObject::tr("时间"));;
        mode->setHeaderData(3,Qt::Horizontal,QObject::tr("状态"));
        q.next();
        if(q.value(0).isNull())
        {
            myHelper::ShowMessageBoxError("无此卡号进出记录.");
            return ;
        }
    }
    else
    {
        mode->setQuery(QString("select *from jfjl where num=\'"+num+"\' order by dt desc")) ;
        QSqlQuery q=mode->query();
        mode->setHeaderData(0,Qt::Horizontal,QObject::tr("卡号"));
        mode->setHeaderData(1,Qt::Horizontal,QObject::tr("缴费时间"));
        mode->setHeaderData(2,Qt::Horizontal,QObject::tr("缴费金额(元)"));;
        mode->setHeaderData(3,Qt::Horizontal,QObject::tr("租赁时长(月)"));
        q.next();
        if(q.value(0).isNull())
        {
            myHelper::ShowMessageBoxError("无此卡号缴费记录.");
            return ;
        }
    }
    ui->tableView_3->setModel(mode);
    ui->tableView_3->resizeColumnsToContents();
}

//固定车位缴费
void Widget::on_pushButton_6_clicked()
{
    QString num=ui->lineEdit_11->text();
    QString time=ui->lineEdit_14->text();
    QString fee=ui->lineEdit_13->text();
    if(num.isEmpty()||time.isEmpty()||fee.isEmpty())
    {
        myHelper::ShowMessageBoxError("警告！输入信息不完整\n请将信息填全.");
        return ;
    }
    QSqlQuery q;
    q.exec("select state,end_time,park from gdcw where num=\'"+num+"\'");
    q.next();
    if(q.value(0).isNull())
    {
        myHelper::ShowMessageBoxError("警告！卡号输入错误.");
        return ;
    }
    int t=time.toInt();
    double f=fee.toDouble();
    QSqlQuery l;
    QDate d=q.value(1).toDate();
    QString park=q.value(2).toString();
    if(q.value(0).toString()=="已过期")
    {
        l.exec("select *from gdcw where state=1 and park=\'"+park+"\'");
        if(l.next())
        {
            l.exec("select *from cw where state=0");
            if(!l.next())
            {
                myHelper::ShowMessageBoxError("续费失败！暂时无空车位");
                return ;
            }
            park=l.value(0).toString();
            myHelper::ShowMessageBoxInfo("缴费成功，原车位已被租赁，此卡号对应新车位为"+park+"");

        }
        d=QDate::currentDate();
        l.exec("update cw set state=1 where cwnum=\'"+park+"\'");
    }
    l.prepare("insert into jfjl values(:num,now(),:f,:t)");
    l.bindValue(":num",num);
    l.bindValue(":f",f);
    l.bindValue(":t",t);
    l.exec();
    d=d.addMonths(t);
    l.prepare("update gdcw set end_time=(:d),state='正常',park=\'"+park+"\' where num=(:num)");
    l.bindValue(":d",d);
    l.bindValue(":num",num);
    bool ok=l.exec();
    if(ok)
    {
        myHelper::ShowMessageBoxInfo("缴费成功！");
        update_tableview_2();
    }
    else
        myHelper::ShowMessageBoxError("警告！数据更新出错");
}

//更改密码
void Widget::on_pushButton_8_clicked()
{
    QString p=ui->lineEdit_16->text();
    QString p1=ui->lineEdit_17->text();
    QString p2=ui->lineEdit_18->text();
    if(p1!=p2)
    {
        myHelper::ShowMessageBoxError("警告！两次新密码输入不一致，请重新输入");
        return;
    }
    QSqlQuery q;
    q.exec("select *from passward");
    q.next();
    QString pmd5;
    QByteArray b,ba;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(p);
    md.addData(ba);
    b = md.result();
    pmd5.append((b.toHex()));
    if(q.value(0).toString()==pmd5)
    {
        pmd5.clear();
        b.clear();
        ba.clear();
        md.reset();
        ba.append(p1);
        md.addData(ba);
        b = md.result();
        pmd5.append((b.toHex()));
        if(!q.exec("update passward set p=\'"+pmd5+"\'"))
        {
            myHelper::ShowMessageBoxError(q.lastError().text());
            return ;
        }
        myHelper::ShowMessageBoxInfo("密码修改成功！");
        return;
    }
    else
        myHelper::ShowMessageBoxError("警告！原密码输入错误");
}

void Widget::on_pushButton_9_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
