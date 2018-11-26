#DEFINES+=IS_CLIENT
DEFINES+=IS_SERVER
HEADERS += \
    test.h configmanager.h

SOURCES += \
    test.cpp configmanager.cpp main.cpp
CONFIG+=c++11
unix{
    DEFINES+=IS_UNIX
}
win32{
    DEFINES+=IS_WIN32
}

DISTFILES += \
    res/config.json


QMAKE_LFLAGS+=-Wl,--rpath=/root/source/opencv-3.2.0/build/__install/lib
INCLUDEPATH+=/root/source/opencv-3.2.0/build/__install/include
LIBS+=-L/root/source/opencv-3.2.0/build/__install/lib
include(common.pri)
