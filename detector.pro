#DEFINES+=IS_CLIENT
DEFINES+=IS_SERVER
HEADERS += \
    test.h configmanager.h videosource.h

SOURCES += \
    test.cpp configmanager.cpp main.cpp videosource.cpp
CONFIG+=c++11
unix{
    DEFINES+=IS_UNIX
}
win32{
    DEFINES+=IS_WIN32
}

DISTFILES += \
    res/config.json
RESOURCES1.path=$$OUT_PWD
RESOURCES1.files=res/
INSTALLS+=RESOURCES1

QMAKE_LFLAGS+=-Wl,--rpath=/root/source/opencv-3.2.0/build/__install/lib
INCLUDEPATH+=/root/source/opencv-3.2.0/build/__install/include
LIBS+=-L/root/source/opencv-3.2.0/build/__install/lib -lopencv_core -lopencv_video -lopencv_videoio -lopencv_imgcodecs
LIBS+=-L/root/source/ffmpeg-3.4.4/__install/lib
LIBS+=-lswresample
include(common.pri)

