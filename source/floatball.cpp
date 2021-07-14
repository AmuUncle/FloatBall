#include "floatball.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QTimerEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QMenu>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QTime>
#include <QProcess>

#ifdef WIN32
#include <windows.h>
#include <sysinfoapi.h>
#endif

const int MAX_WINDOWS_WIDTH = 64;
const int MIN_WINDOWS_WIDTH = 10;

FloatBall::FloatBall(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setProperty("canMove", "true");
    setProperty("forbidMoveOtherScreen", "true");
    setCursor(Qt::PointingHandCursor); //设置鼠标样式

    m_dwMemoryLoad = 0;
    m_bIsMini = false;
    m_bIsCleaning = false;
    m_nDirection = 0;
    m_nCleanTimerId = -1;

    CreateAllChildWnd();
    InitCtrl();
    InitSolts();
    Relayout();
    OnUpdateMemInfo();
}

FloatBall::~FloatBall()
{

}

void FloatBall::CreateAllChildWnd()
{

}

void FloatBall::InitCtrl()
{
    setFixedSize(MAX_WINDOWS_WIDTH, MAX_WINDOWS_WIDTH);
    m_nUpdateMemTimerId = startTimer(1000);
}

void FloatBall::InitSolts()
{

}

void FloatBall::Relayout()
{

}

void FloatBall::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);         // 创建画家对象
    QRect rcClient = rect();

    if (m_bIsMini)
    {
        QColor boderColor("#888888");
        QColor bgColor("#000000");
        QColor textColor("#16D25E");

        painter.save();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);
        painter.setPen(Qt::NoPen);
        painter.setBrush(boderColor);
        painter.drawRoundedRect(rcClient.marginsRemoved(QMargins(0, 0, 0, 0)), 20, 3);
        painter.setBrush(bgColor);
        painter.drawRoundedRect(rcClient.marginsRemoved(QMargins(1, 1, 1, 1)), 20, 3);
        painter.restore();

        painter.save();
        QRect rcBase = rcClient.marginsRemoved(QMargins(2, 2, 2, 2));
        painter.setPen(QPen(textColor, 1));

        QPoint p1, p2;
        for (int i = 0; i < (rcBase.height() * (double)m_dwMemoryLoad/100.0); i+=2)
        {
            if (0 == i || (rcBase.bottom() - i) == rcBase.height())
            {
                p1.setX(rcBase.left() + 1);
                p2.setX(rcBase.right() - 1);
                p1.setY(rcBase.bottom() - i);
                p2.setY(rcBase.bottom() - i);
                painter.drawLine(p1, p2);

                p1.setX(rcBase.left());
                p2.setX(rcBase.right());
                continue;
            }

            p1.setY(rcBase.bottom() - i);
            p2.setY(rcBase.bottom() - i);
            painter.drawLine(p1, p2);
        }

        painter.restore();
    }
    else
    {
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);

        QColor boderColor("#FFFFFF");
        boderColor.setAlpha(100);
        QColor bgColor("#FFFFFF");
        QColor brandColor("#56E56C");
        QColor textColor("#333333");

        if (m_bIsCleaning)
        {
            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#5CD43E"));
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(0, 0, 0, 0)));
            painter.setBrush(bgColor);

            switch (m_nDirection % 4)
            {
            case 0:
                {
                    painter.drawEllipse(rcClient.marginsRemoved(QMargins(3, 2, 2, 2)));
                }
                break;
            case 1:
                {
                    painter.drawEllipse(rcClient.marginsRemoved(QMargins(2, 3, 2, 2)));
                }
                break;
            case 2:
                {
                    painter.drawEllipse(rcClient.marginsRemoved(QMargins(2, 2, 3, 2)));
                }
                break;
            case 3:
                {
                    painter.drawEllipse(rcClient.marginsRemoved(QMargins(2, 2, 2, 4)));
                }
                break;
            }

            painter.setBrush(QColor("#5CD43E"));
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(3, 3, 3, 3)));

            painter.setBrush(bgColor);
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(5, 5, 5, 5)));
            painter.restore();

            painter.save();
            QFont font = painter.font();
            font.setPointSize(16);
            painter.setFont(font);
            painter.setPen(textColor);
            painter.drawText(rcClient.marginsRemoved(QMargins(5, (m_nDirection % 2) ?  4 : 6, 5, 5)), Qt::AlignCenter, QString("%1%").arg(m_dwMemoryLoad));
            painter.restore();
        }
        else
        {
            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(boderColor);
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(0, 0, 0, 0)));
            painter.setBrush(brandColor);
            QRect rcangle(rcClient.marginsRemoved(QMargins(0, 0, 0, 0)));//设定扇形所在的矩形，扇形画在矩形内部
            int startAngle = 90 * 16;
            int spanAngle = -360 * ((double)m_dwMemoryLoad/100.0) * 16;
            painter.drawPie(rcangle, startAngle, spanAngle);

            painter.save();
            painter.setPen(brandColor);
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(0, 0, 0, 0)));
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(5, 5, 5, 5)));
            painter.restore();

            painter.setBrush(bgColor);
            painter.drawEllipse(rcClient.marginsRemoved(QMargins(5, 5, 5, 5)));
            painter.restore();

            painter.save();
            QFont font = painter.font();
            font.setPointSize(16);
            painter.setFont(font);
            painter.setPen(textColor);
            painter.drawText(rcClient.marginsRemoved(QMargins(5, 5, 5, 20)), Qt::AlignCenter, QString("%1%").arg(m_dwMemoryLoad));
            painter.restore();

            painter.save();
            painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, false);
            QPoint p1, p2;
            p1.setX(rcClient.left() + 15);
            p1.setY(rcClient.bottom() - 22);
            p2.setX(rcClient.left() + 15);
            p2.setY(rcClient.bottom() - 15);

            painter.setPen(QPen(brandColor, 1));
            painter.drawLine(p1, p2);

            painter.setBrush(brandColor);

            qsrand(QTime::currentTime().msec());

            QPolygon trianglePolygon;

            if (qrand() % 2)
            {
                trianglePolygon << QPoint(p2.x() - 3, p2.y() - 3);
                trianglePolygon << QPoint(p2.x() + 3, p2.y() - 3);
                trianglePolygon << QPoint(p2);
            }
            else
            {
                trianglePolygon << QPoint(p1.x() - 3, p1.y() + 3);
                trianglePolygon << QPoint(p1.x() + 3, p1.y() + 3);
                trianglePolygon << QPoint(p1);
            }

            painter.drawPolygon(trianglePolygon);
            painter.restore();

            painter.save();
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);
            font.setPointSize(7);
            painter.setFont(font);
            painter.setPen(textColor);
            painter.drawText(rcClient.marginsRemoved(QMargins(10, 32, 5, 5)), Qt::AlignCenter, QString("%1K/s").arg(qrand() % 100));
            painter.restore();
        }
    }
}

void FloatBall::timerEvent(QTimerEvent *event)
{
    if (m_nUpdateMemTimerId == event->timerId())
    {
        OnUpdateMemInfo();
    }
    else if (m_nLeaveTimerId  == event->timerId())
    {
        killTimer(m_nLeaveTimerId);
        m_nLeaveTimerId = -1;

        int screenWidth = QApplication::desktop()->screenGeometry().width();
        if(this->geometry().x() + this->width() > screenWidth - 2)
        {
            QRect rect = this->geometry();
            rect.setX(screenWidth - MIN_WINDOWS_WIDTH);

            QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
            pAnimation->setStartValue(this->geometry());
            pAnimation->setEndValue(rect);
            pAnimation->setEasingCurve(QEasingCurve::Linear);
            pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

            connect(pAnimation, &QPropertyAnimation::finished, [=]()
            {
                this->setFixedWidth(MIN_WINDOWS_WIDTH);
                m_bIsMini = true;
            });
        }

        update();
    }
    else if (m_nCleanTimerId  == event->timerId())
    {
        if (m_nDirection < 200)
        {
            m_nDirection++;
        }
        else
        {
            m_bIsCleaning = false;
            m_nDirection = 0;

            if (m_nCleanTimerId > -1)
                killTimer(m_nCleanTimerId);

            m_nCleanTimerId = -1;
            setProperty("canMove", "true");
        }

        update();
    }
}

void FloatBall::enterEvent(QEvent *event)
{
    if (m_nLeaveTimerId > -1)
    {
        killTimer(m_nLeaveTimerId);
        m_nLeaveTimerId = -1;
    }

    m_bIsMini = false;
    setFixedSize(MAX_WINDOWS_WIDTH, MAX_WINDOWS_WIDTH);

    int screenWidth = QApplication::desktop()->screenGeometry().width();
    if(this->geometry().x() + this->width() > screenWidth)
    {
        QRect rect = this->geometry();
        rect.setX(screenWidth - this->width());

        QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
        pAnimation->setStartValue(this->geometry());
        pAnimation->setEndValue(rect);
        pAnimation->setEasingCurve(QEasingCurve::Linear);
        pAnimation->setDuration(100);
        pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    update();
}

void FloatBall::leaveEvent(QEvent *event)
{
    m_nLeaveTimerId = startTimer(1000);
}

void FloatBall::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_nCleanTimerId > -1)
        return;

    m_bIsCleaning = true;
    m_nDirection = 0;
    m_nCleanTimerId = startTimer(10);
    setProperty("canMove", "false");
}

void FloatBall::OnUpdateMemInfo()
{
#ifdef WIN32
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(memoryInfo);
    GlobalMemoryStatusEx(&memoryInfo);
    m_dwMemoryLoad = memoryInfo.dwMemoryLoad;
#else

    //QProcess用于启动外部程序
    QProcess process;
    //执行ls命令
    process.start("free");
    //等待命令执行结束
    process.waitForFinished();
    //获取命令执行的结果
    QByteArray result_ = process.readAllStandardOutput();

    QString strRet(result_);
    if (strRet.split('\n').length() > 1)
    {
        QString strMem = strRet.split('\n')[1];
        strMem = strMem.replace(QRegExp("\\s{1,}"), " ");
        QStringList strList = strMem.split(' ');

        if (strList.length() > 2)
            m_dwMemoryLoad = strList[2].toDouble() / strList[1].toDouble() * 100;
    }
#endif

    update();
}
