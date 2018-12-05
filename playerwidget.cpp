#include "playerwidget.h"
#include <QTimer>
//PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent),src("rtsp://192.168.2.103:8554/live/test1")
PlayerWidget::PlayerWidget(Camera_Pri data,QWidget *parent) : QOpenGLWidget(parent)
  //PlayerWidget::PlayerWidget(CameraInputData data,QWidget *parent) : QWidget(parent),src("/root/test.mp4")
{
    src=new VideoSource(data.Url);
    set_delay(800/40);//  800ms is default delay on ui
//    show_input=true;
//    show_output=true;
    delay_frames=0;

    camera_data=data;
    tick_timer=new QTimer();
    connect(tick_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(&check_timer,SIGNAL(timeout()),this,SLOT(check_point()));
    connect(&menu,SIGNAL(aboutToHide()),this,SLOT(clear_menu()),Qt::QueuedConnection);
    tick_timer->start(100);//check frame aviliable every 100ms
    check_timer.start(1000);
}
