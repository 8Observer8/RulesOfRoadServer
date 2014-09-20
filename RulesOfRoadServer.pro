#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T11:45:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RulesOfRoadServer
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Server.cpp

HEADERS  += MainWindow.h \
    Server.h

FORMS    += MainWindow.ui
