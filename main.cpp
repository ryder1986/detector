/*
 1. do i need

*/

//#include <iostream>
////#include "test.h"
//#include "device_manager.h"
//#include "configmanager.h"
//int main()
//{
//    ConfigManager cfg;
//    Device_Manager dm(Device_Pri(cfg.get_config()));
//    PAUSE_HERE_FOREVER
//    return 0;
//}



#include <QApplication>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();

}
