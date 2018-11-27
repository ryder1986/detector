CPPJSON_PATH=cppjson/
INCLUDEPATH+=$$CPPJSON_PATH/include
SOURCES +=\
    $$CPPJSON_PATH/json_reader.cpp \
    $$CPPJSON_PATH/json_writer.cpp \
    $$CPPJSON_PATH/json_value.cpp
HEADERS+=tool.h

SOURCES+=tool.cpp
