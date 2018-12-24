#-------------------------------------------------
#
# Project created by QtCreator 2018-11-30T10:58:59
#
#-------------------------------------------------


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testvideosource
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp player.cpp

HEADERS  += mainwindow.h player.h

FORMS    += mainwindow.ui
CONFIG+=c++11



########comon components##########
COMMON_FILES_PATH=./../../
#CONFIG +=WITH_CPPJSON
#CONFIG+=WITH_WIDGET
CONFIG+=WITH_OPENCV
include($$COMMON_FILES_PATH/common.pri)
