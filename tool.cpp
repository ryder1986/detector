#include "tool.h"
mutex Tool1::lock;
int Tool1::lv=Tool1::DEBUG_LEVEL::INFO;
string Tool1::match_label=string();
int Tool1::label_printable=Tool1::LABEL_SELECTION::BOTH;//1:only print "debug to fatal" label 2:only print user-defined label 3:print both "debug to fatal" and user-defined label 4:only match match_label
LogFile Tool1::log_file1("log_file1.txt");
//LogFile Tool1::log_file1;
bool ClientConfig::show_processor_text=true;
bool ClientConfig::show_camera_state=false;
bool ClientConfig::show_output=true;
bool ClientConfig::show_input=true;
