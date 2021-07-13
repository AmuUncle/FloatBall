﻿#include "appinit.h"
#include "qmutex.h"
#include "qapplication.h"
#include "qevent.h"
#include "qwidget.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>

AppInit *AppInit::self = 0;
AppInit *AppInit::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new AppInit;
        }
    }

    return self;
}

AppInit::AppInit(QObject *parent) : QObject(parent)
{
}

bool AppInit::eventFilter(QObject *obj, QEvent *evt)
{
    QWidget *w = (QWidget *)obj;
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(obj, evt);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - w->pos();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = false;
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() && Qt::LeftButton))
        {
            if (w->property("forbidMoveOtherScreen").toBool())
            {
                int screenWidth = QApplication::desktop()->screenGeometry().width();
                QPoint move_pos = event->globalPos();
                QPoint tarPos = move_pos - mousePoint;

                if(tarPos.x() + w->width() > screenWidth)
                {
                    tarPos.setX(screenWidth - w->width());
                }
                w->move(tarPos);
            }
            else
            {
                w->move(event->globalPos() - mousePoint);
            }


            return true;
        }
    }

    return QObject::eventFilter(obj, evt);
}

void AppInit::start()
{
    qApp->installEventFilter(this);
}
