#-------------------------------------------------
#
# Project created by QtCreator 2021-07-13T14:58:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 360FloatBall
TEMPLATE = app

include ($$PWD/public/public.pri)

INCLUDEPATH += $$PWD/include

SOURCES += source/main.cpp\
        source/floatball.cpp

HEADERS  += include/floatball.h
