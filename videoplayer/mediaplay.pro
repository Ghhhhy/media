#-------------------------------------------------
#
# Project created by QtCreator 2018-12-19T19:59:54
#
#-------------------------------------------------

QT       += core gui sql
QT += multimedia
QT += multimediawidgets

RC_ICONS = bitbug_favicon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mediaplay
TEMPLATE = app

CONFIG += resources_big
SOURCES += main.cpp\
        customslider.cpp \
        mediaplay.cpp \
    qsqlitedb.cpp

HEADERS  += mediaplay.h \
    customslider.h \
    qsqlitedb.h

FORMS    += mediaplay.ui

RESOURCES += \
    src.qrc
 #    image.qrc
