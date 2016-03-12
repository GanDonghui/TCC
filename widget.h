#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "dialog.h"
#include <QApplication>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    friend class Diolog;

public:
    explicit Widget(QWidget *parent = 0);

    void update_tableview();

    void update_tableview_2();

    void update_tableview_5();

    void update_view();

    bool update_sfsz(QTime st,QTime et,double f);

    ~Widget();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    void mouseMoveEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_pushButtoni_in_clicked();

    void on_pushButton_out_clicked();

    void on_pushButton_lscw1_clicked();

    void on_pushButton_lscw2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_gdcw1_clicked();

    void on_gdcw2_clicked();

    void on_pushButton_lscw3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void update_lcd();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_btnMenu_Close_clicked();

    void on_btnMenu_Max_clicked();

    void on_btnMenu_Min_clicked();

    void on_btnMenu_clicked();


private:
    Ui::Widget *ui;
    Dialog d;
    QTimer *timer;

    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;

    void InitStyle();
};

#endif // WIDGET_H
