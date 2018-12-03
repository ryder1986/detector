#DEFINES+=IS_CLIENT
DEFINES+=IS_SERVER
HEADERS += \
    test.h configmanager.h

SOURCES += \
    test.cpp configmanager.cpp main.cpp
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
CONFIG +=WITH_CPPJSON
CONFIG+=WITH_OPENCV
include($$COMMON_FILES_PATH/common.pri)

