#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include "videosource.h"
#include <qmutex.h>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include "opencv2/opencv.hpp"
//#include "camera_data.h"
#include <QMenu>
#include <QAction>
#include "jsonpacket.h"
#include "device_manager.h"
class MyAction:public QAction
{
    Q_OBJECT
public:
    MyAction(QWidget *w):QAction(w)
    {
        connect(this,SIGNAL(triggered(bool)),this,SLOT(trig(bool)));
    }

    MyAction(RequestPkt pk,string text,QWidget *w):QAction(w)
    {
        connect(this,SIGNAL(triggered(bool)),this,SLOT(trig(bool)));

        // connect(this,SIGNAL(()),this,SLOT(dest(QObject *)));

        txt=text;
        pkt=pk;
        checked=false;
        checkable=false;
        this->setText(QString(txt.data()));
    }
    MyAction(bool chkd,bool chka,RequestPkt pk,string text,QWidget *w):QAction(w)
    {
        connect(this,SIGNAL(triggered(bool)),this,SLOT(trig(bool)));


        // connect(this,SIGNAL(()),this,SLOT(dest(QObject *)));

        txt=text;
        pkt=pk;
        checked=chkd;
        checkable=chka;
        this->setText(QString(txt.data()));
    }
    void set_checked(bool chkd)
    {
        checked=chkd;

        this->setChecked(chkd);
    }
    void set_checkable(bool chka)
    {
        checkable=chka;
        this->setCheckable(chka);
    }

signals:
    void choose(MyAction *p);

public slots:
    void dest(QObject *)
    {
        prt(info,"des %s",txt.data());
    }
    void trig(bool checked)
    {
        prt(info,"checked ?  %d",checked);
        emit choose(this);
    }
public:
    RequestPkt pkt;
    string txt;
    bool checked;
    bool checkable;


};
class PlayerWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(Camera_Pri data,QWidget *parent = nullptr);
    ~PlayerWidget()
    {
        lock.lock();

        tick_timer->stop();
        delete tick_timer;
        lock.unlock();
        //  VideoSource *tmp=src;
        // std::thread([tmp](){ delete tmp;}).detach();
        //  delete src;


        DELETE_POINTER_LATER(VideoSource*,src,1);
    }

    void set_output_data(Camera_Output data)
    {
        // lock.lock();
        output_lock.lock();
        data_fps++;
        output_data=data;
        output_lock.unlock();
        //   lock.unlock();
    }
    inline void  get_test_img()
    {
        cv::Mat bgr_frame;
        cv::Mat rgb_frame;
        int ts;
        bgr_frame =cv::imread("res/no_image.png");
        //prt(info,"get ts %d",ts);
        if(!bgr_frame.empty()){
            cv::cvtColor(bgr_frame,rgb_frame,CV_BGR2RGB);
            cv::putText(rgb_frame,string("opening"),cv::Point(100,100),1,2,cv::Scalar(3),5);
            img=QImage((const uchar*)(rgb_frame.data),
                       rgb_frame.cols,rgb_frame.rows,
                       QImage::Format_RGB888);
            img.bits();
        }else{
            throw exception();
        }
        //  return img;
    }
    void set_delay(int frames)
    {
        delay_frames=frames;
        src->set_buffer_size(frames);
    }

protected:
    void paintEvent(QPaintEvent *)
    {
#if 1
        int outflag=false;
        lock.lock();
        try{
            // lock.lock();
            QPainter this_painter(this);
            if(!get_img()){
                //img=
                get_test_img();

                lock.unlock();
                return;
            }
            QPainter img_painter(&img);
            current_painter=&img_painter;
#if 0
            if(ClientConfig::show_input){
                // draw input
                camera_data.draw(bind(&PlayerWidget::draw_line,
                                      this,placeholders::_1,
                                      placeholders::_2,placeholders::_3,placeholders::_4),
                                 bind(&PlayerWidget::draw_circle,
                                      this,placeholders::_1,
                                      placeholders::_2,placeholders::_3,placeholders::_4),
                                 bind(&PlayerWidget::draw_text,
                                      this,placeholders::_1,
                                      placeholders::_2,placeholders::_3,placeholders::_4,
                                      placeholders::_5)
                                 );
            }
#endif
            // lock.unlock();

            // if(0){
            if(ClientConfig::show_output){
                // draw output
                output_lock.lock();
                if(camera_data.data().str().size()>10&&\
                        output_data.data().str().size()>0){//TODO:better way?

                    output_data_tmp=output_data;
                    outflag=true;

                }else{

                }
                output_lock.unlock();

#if 0
                if(ClientConfig::show_output&&outflag)
                    output_data_tmp.draw(
                                camera_data,
                                bind(&PlayerWidget::draw_line,
                                     this,placeholders::_1,
                                     placeholders::_2,placeholders::_3,placeholders::_4),
                                bind(&PlayerWidget::draw_circle,
                                     this,placeholders::_1,
                                     placeholders::_2,placeholders::_3,placeholders::_4),
                                bind(&PlayerWidget::draw_text,
                                     this,placeholders::_1,
                                     placeholders::_2,placeholders::_3,placeholders::_4,
                                     placeholders::_5)

                                );
#endif


            }

            // lock.lock();
            if(ClientConfig::show_camera_state){
                draw_text(QString("data fps:").append(QString::number(data_fps_old)).toStdString(),Point_Pri(200,340),100,PaintableData::Yellow,10);

            }

#if 0
            draw_text(QString("img ts: ").append(QString::number(timestamp)).toStdString().data(),VdPoint(200,320),100,PaintableData::Red,30);
            draw_text(QString("data ts late for : ").append(QString::number(timestamp-output_data.Timestamp)).append(" ms").toStdString().data(),VdPoint(200,350),100,PaintableData::Red,30);
#endif
            if(!img.isNull()){
                this_painter.drawImage(QRect(0,0,this->width(),this->height()),img);
            }
            // lock.unlock();
        }
        catch(exception e){
            prt(info,"exception in image");
            // lock.unlock();
        }

        lock.unlock();

        //  this_thread::sleep_for(chrono::milliseconds(100));
        //paint_lock.lock();

        //   this_thread::sleep_for(chrono::milliseconds(100));

        //        if(ClientConfig::show_output&&outflag)
        //            output_data_tmp.draw(
        //                        camera_data,
        //                        bind(&PlayerWidget::draw_line,
        //                             this,placeholders::_1,
        //                             placeholders::_2,placeholders::_3,placeholders::_4),
        //                        bind(&PlayerWidget::draw_circle,
        //                             this,placeholders::_1,
        //                             placeholders::_2,placeholders::_3,placeholders::_4),
        //                        bind(&PlayerWidget::draw_text,
        //                             this,placeholders::_1,
        //                             placeholders::_2,placeholders::_3,placeholders::_4,
        //                             placeholders::_5)

        //                        );
        //       paint_lock.unlock();
#endif
    }
    inline void draw_line(Point_Pri s,Point_Pri e,int colour,int size)
    {

        switch (colour) {
        case PaintableData::Colour::Green:
            current_painter->setPen(QPen(QBrush(QColor(0,255,0)),size));
            break;
        case PaintableData::Colour::Red:
            current_painter->setPen(QPen(QBrush(QColor(255,0,0)),size));
            break;
        case PaintableData::Colour::Blue:
            current_painter->setPen(QPen(QBrush(QColor(0,100,255)),size));
            break;
        case PaintableData::Colour::Yellow:
            current_painter->setPen(QPen(QBrush(QColor(255,255,100)),size));
            break;
        default:
            break;
        }

        QPen pen_ori=current_painter->pen();
        current_painter->drawLine(QPoint(s.x,s.y),QPoint(e.x,e.y));
        current_painter->setPen(pen_ori);
    }
    inline void draw_circle(Point_Pri center,int rad,int colour,int size)
    {

        switch (colour) {
        case PaintableData::Colour::Green:
            current_painter->setPen(QPen(QBrush(QColor(0,255,0)),size));
            break;
        case PaintableData::Colour::Red:
            current_painter->setPen(QPen(QBrush(QColor(255,0,0)),size));
            break;
        case PaintableData::Colour::Blue:
            current_painter->setPen(QPen(QBrush(QColor(0,100,255)),size));
            break;
        case PaintableData::Colour::Yellow:
            current_painter->setPen(QPen(QBrush(QColor(255,255,100)),size));
            break;
        default:
            break;
        }

        QPen pen_ori=current_painter->pen();
        current_painter->drawEllipse(QPoint(center.x,center.y),rad,rad);
        current_painter->setPen(pen_ori);
    }
    inline void draw_text(string text,Point_Pri center,int rad,int colour,int size)
    {

        switch (colour) {
        case PaintableData::Colour::Green:
            current_painter->setPen(QPen(QBrush(QColor(0,255,0)),size));
            break;
        case PaintableData::Colour::Red:
            current_painter->setPen(QPen(QBrush(QColor(255,0,0)),size));
            break;
        case PaintableData::Colour::Blue:
            current_painter->setPen(QPen(QBrush(QColor(0,100,255)),size));
            break;
        case PaintableData::Colour::Yellow:
            current_painter->setPen(QPen(QBrush(QColor(255,255,100)),size));
            break;
        default:
            break;
        }

#if 0
        QFont font;
       // font.setPointSize(current_painter->window().height()/30);
        font.setPointSize(current_painter->window().height()/90);
        font.setFamily("Microsoft YaHei");
        font.setLetterSpacing(QFont::AbsoluteSpacing,0);
        current_painter->setFont(font);

#endif
        QPen pen_ori=current_painter->pen();
        current_painter->drawText(center.x,center.y,QString(text.data()));
        current_painter->setPen(pen_ori);
    }
    void  initializeGL()
    {

    }


signals:
    void camera_request(RequestPkt req,PlayerWidget *w);
    void double_click_event(PlayerWidget *w);
public slots:

    void timeout()
    {
        this->update();
    }
    void check_point()
    {
        // data_fps++;
        data_fps_old=data_fps;
        data_fps=0;
    }
    void choose_method(bool )
    {
        prt(info,"choose method");

    }    void choose_method1( )
    {
        prt(info,"choose method ss  ");

    }
    void choose_item (MyAction *act )
    {
        // if(act->checked){
        prt(info,"%s select",act->text().toStdString().data());
        prt(info,"sending -->>>> %s",act->pkt.data().str().data());
        emit camera_request(act->pkt,this);
        //}
        //  clear_menu();
    }
    void clear_menu()
    {
        prt(info,"clear menu");
        for(QAction *a:actions){
            menu.removeAction(a);
            delete a;
        }
        actions.clear();
    }
#if 0
    void mousePressEvent(QMouseEvent *e)
    {
        prt(info,"mouse press");
        if(e->button()==Qt::RightButton){
            vector <RequestPkt> reqs;
            vector <string> texts;
            camera_data.right_press(QPoint_2_VdPoint(map_point(e->pos())),reqs,texts);

            if(reqs.size()>0&&reqs.size()==texts.size()){
                for(int i=0;i<reqs.size();i++){
                    MyAction *ma=new MyAction(reqs[i],texts[i],this);
                    ma->setCheckable(true);
                    ma->setChecked(true);
                    connect(ma,&MyAction::choose,this,&PlayerWidget::choose_item,Qt::DirectConnection);

                    actions.push_back(ma);
                    menu.addAction(ma);

                }

            }
            menu.exec(QCursor::pos());

        }else{
            camera_data.press(QPoint_2_VdPoint((map_point(e->pos()))));
        }
        //        vector <DetectRegionInputData >detect_regions;
        //        detect_regions.assign(cfg.DetectRegion.begin(),cfg.DetectRegion.end());

    }
#else
    void mousePressEvent(QMouseEvent *e)
    {
        prt(info,"mouse press");
#if 0
        if(e->button()==Qt::RightButton){
            // vector <RequestPkt> reqs;
            //  vector <string> texts;
            vector <right_press_menu_item>  itms;
            camera_data.right_press(QPoint_2_VdPoint(map_point(e->pos())),itms);

            if(itms.size()>0){
                for(int i=0;i<itms.size();i++){
                    MyAction *ma=new MyAction(itms[i].pkt,itms[i].text,this);

                    ma->set_checkable(itms[i].checkable);
                    ma->set_checked(itms[i].checked);
                    //                    if(itms[i].checkable)

                    //                        ma->setCheckable(true);
                    //                    if(itms[i].checked)

                    //                        ma->setChecked(true);
                    connect(ma,&MyAction::choose,this,&PlayerWidget::choose_item,Qt::DirectConnection);

                    actions.push_back(ma);
                    menu.addAction(ma);

                }

            }
            menu.exec(QCursor::pos());

        }else{
            camera_data.press(QPoint_2_VdPoint((map_point(e->pos()))));
        }
#endif
        //        vector <DetectRegionInputData >detect_regions;
        //        detect_regions.assign(cfg.DetectRegion.begin(),cfg.DetectRegion.end());

    }
#endif
    void mouseMoveEvent(QMouseEvent *e)
    {
#if 0
        QPoint p1=map_point(e->pos());
        camera_data.move(QPoint_2_VdPoint((map_point(e->pos()))));
#endif
    }

    void mouseReleaseEvent(QMouseEvent *e)
    {
        prt(info,"mouse release");
#if 0
        RequestPkt req1;
        if( camera_data.release(req1)){
            emit camera_request(req1,this);
        }
#endif
    }

    void mouseDoubleClickEvent(QMouseEvent *e)
    {
        prt(info,"double");
        emit double_click_event(this);//TODO:set full screen?
    }
    void show_text_info(bool show)
    {
        ClientConfig::show_processor_text=show;
    }

private:
    inline QPoint map_point(QPoint p)
    {
        return QPoint(p.x()*img.width()/this->width(),p.y()*img.height()/this->height());
    }
    inline Point_Pri QPoint_2_VdPoint(QPoint p)
    {
        return Point_Pri(p.x(),p.y());
    }
    bool  get_img()
    {
        cv::Mat rgb_frame;
        cv::Mat bgr_frame;
        int ts;
        //   bool ret=src.get_frame(bgr_frame);
        bool ret=src->get_frame(bgr_frame,ts);
        timestamp=ts;
        //prt(info,"get ts %d",ts);
        if(ret){
            cv::cvtColor(bgr_frame,rgb_frame,CV_BGR2RGB);
            img=QImage((const uchar*)(rgb_frame.data),
                       rgb_frame.cols,rgb_frame.rows,
                       QImage::Format_RGB888);
            img.bits();
        }
        return ret;
    }
    VideoSource *src;
    QMutex lock;
    QMutex output_lock;
    QMutex paint_lock;
    QImage img;
    int timestamp;
    QTimer *tick_timer;
    QTimer check_timer;
    Camera_Pri camera_data;
    Camera_Output output_data;
    Camera_Output output_data_tmp;
    QPainter *current_painter;
    vector< MyAction *> actions;
    QMenu menu;
    //    bool show_input;
    //    bool show_output;
    //bool show_text;
    int delay_frames;
    int data_fps;
    int data_fps_old;
};

#endif // PLAYERWIDGET_H
