WITH_CPPJSON{
CPPJSON_PATH=cppjson/
INCLUDEPATH+=$$CPPJSON_PATH/include
SOURCES +=\
    $$CPPJSON_PATH/json_reader.cpp \
    $$CPPJSON_PATH/json_writer.cpp \
    $$CPPJSON_PATH/json_value.cpp
}
WITH_OPENCV{
QMAKE_LFLAGS+=-Wl,--rpath=/root/source/opencv-3.2.0/build/__install/lib
INCLUDEPATH+=/root/source/opencv-3.2.0/build/__install/include
LIBS+=-L/root/source/opencv-3.2.0/build/__install/lib -lopencv_core -lopencv_video -lopencv_videoio -lopencv_imgcodecs
LIBS+=-L/root/source/ffmpeg-3.4.4/__install/lib
LIBS+=-lswresample

}
equals(COMMON_FILES_PATH, ""){
    message("COMMON_FILES_PATH is not set...")
}


HEADERS+=$$COMMON_FILES_PATH/tool.h $$COMMON_FILES_PATH/videosource.h
SOURCES+=$$COMMON_FILES_PATH/tool.cpp $$COMMON_FILES_PATH/videosource.cpp


win32{
    DEFINES+=IS_WIN
}
unix{
    DEFINES+=IS_UNIX
}
