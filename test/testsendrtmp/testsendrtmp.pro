TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
LIBS+=-lpthread

LIBS+=-lrtmp


########comon components##########
COMMON_FILES_PATH=./../../
#CONFIG +=WITH_CPPJSON
CONFIG+=WITH_OPENCV
CONFIG+=WITH_FFMPEG
include($$COMMON_FILES_PATH/common.pri)

HEADERS += \
    test.h encode.h rtmp.h

