/**
  ******************************************************************************
  * @file    widget.cpp
  * @version V1.0
  * @date    2022-02-15
  * @brief   Qt电子时钟程序
  ******************************************************************************
  * @attention Win10编译或嵌入式平台交叉编译
  *
  * 实验平台:Win10/imx6ULL嵌入式Linux开发板
  * 公众号  :码农爱学习(Coder-love-study)
  * 个人博客:https://xxpcb.gitee.io
  *
  ******************************************************************************
  */

#include "widget.h"
#include<QPainter>
#include<QTimer>
#include<QTime>
#include<QString>
#include<QVector>
#include<QMap>

#define CLOCK_RADIUS (80) //时钟的半径
#define PANEL_RADIUS_NUM (3) //表盘的3个圆
#define PANEL_RADIUS1 CLOCK_RADIUS //圆1的半径
#define PANEL_RADIUS2 (CLOCK_RADIUS - 6) //圆2的半径
#define PANEL_RADIUS3 (CLOCK_RADIUS - 8) //圆3的半径
#define HOUR_NUM_SIZE (10) //小时数字的字体大小

//3个表针的形状（三角形）
static QPoint hourHand[3] = {
    QPoint(5, 3),
    QPoint(-5, 3),
    QPoint(0, -30)
};
static QPoint minuteHand[3] = {
    QPoint(4, 6),
    QPoint(-4, 6),
    QPoint(0, -45)
};
static QPoint secondHand[3] = {
    QPoint(2, 10),
    QPoint(-2, 10),
    QPoint(0, -60)
};

//表针与刻度颜色
static QColor hourColor(255, 0, 0);
static QColor minuteColor(0, 0, 255);
static QColor secondColor(0, 255, 0);

//表盘参数
struct panelPara{
    int radius;
    QColor color;
};
//圆的半径与对于的颜色
static panelPara stPanelParaArr[] = {
    {PANEL_RADIUS1, QColor(255, 200, 100)},
    {PANEL_RADIUS2, QColor(164, 211, 238)},
    {PANEL_RADIUS3, QColor(255, 255, 255)},
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    setWindowTitle(tr("Clock"));
    setMinimumSize(200, 200); //设置最小尺寸
}

Widget::~Widget()
{

}


void Widget::paintEvent(QPaintEvent *event)
{
    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width()/2, height()/2); //画图的基准位置
    painter.scale(side/200.0, side/200.0); //随窗口尺寸自动缩放

    //表盘
    for (int i=0; i<PANEL_RADIUS_NUM; i++)
    {
        QBrush brush(stPanelParaArr[i].color);
        QPen pen(stPanelParaArr[i].color);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawEllipse(-stPanelParaArr[i].radius, -stPanelParaArr[i].radius, 2*stPanelParaArr[i].radius, 2*stPanelParaArr[i].radius);
    }

    //小时的表针
    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);

    painter.save();
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();

    //小时的刻度
    painter.setPen(hourColor);
    for (int i = 0; i < 12; ++i)
    {
        painter.rotate(30.0);
        painter.drawLine(PANEL_RADIUS3-6, 0, PANEL_RADIUS3, 0);
        QFont font("TimesNewRoman", HOUR_NUM_SIZE);
        painter.setFont(font);
        painter.drawText(-HOUR_NUM_SIZE, -(CLOCK_RADIUS-15), 2*HOUR_NUM_SIZE, 2*HOUR_NUM_SIZE, Qt::AlignHCenter, QString::number(i+1));
    }

    //分钟的表针
    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    painter.setPen(minuteColor);
    for (int j = 0; j < 60; ++j)
    {
        if ((j % 5) != 0)
        {
            painter.drawLine(PANEL_RADIUS3-4, 0, PANEL_RADIUS3, 0);
        }
        painter.rotate(6.0);
    }

    //秒钟的表针
    painter.setPen(Qt::NoPen);
    painter.setBrush(secondColor);

    painter.save();
    painter.rotate(6.0 * time.second());
    painter.drawConvexPolygon(secondHand, 3);
    painter.restore();

    painter.end();
}
