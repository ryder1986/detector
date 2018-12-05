
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


#DEFINES+=IS_CLIENT
DEFINES+=IS_SERVER
HEADERS += \
     configmanager.h device_manager.h cameras_manager.h camera_manager.h region_manager.h processor_manager.h \
    data_manager.h \
    mainwindow.h playerwidget.h

SOURCES += \
 configmanager.cpp main.cpp \
    mainwindow.cpp playerwidget.cpp
CONFIG+=c++11
unix{
    DEFINES+=IS_UNIX
    LIBS+=-lpthread
}
win32{
    DEFINES+=IS_WIN32
}

DISTFILES += \
    res/config.json
RESOURCES1.path=$$OUT_PWD
RESOURCES1.files=res/
INSTALLS+=RESOURCES1





########comon components##########
COMMON_FILES_PATH=./
CONFIG+=WITH_CPPJSON
CONFIG+=WITH_OPENCV
include($$COMMON_FILES_PATH/common.pri)

FORMS += \
    mainwindow.ui
