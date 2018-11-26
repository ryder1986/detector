#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "tool.h"
#ifdef IS_UNIX
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#endif
#ifdef IS_WIN
#include <opencv2/opencv.hpp>
#endif
#if 1
#define USE_CVCAP 1
//#undef USE_CVCAP
#ifdef  USE_CVCAP
#else
#include "ffmpegvideocapture.h"
#endif
using namespace std;
using namespace cv;
class VideoSource
{
public:
    VideoSource(string path);
    VideoSource(string path,bool only_key_frame);
     ~VideoSource();
    void set_buffer_size(int frames)
    {
        queue_length=frames;
    }

    inline string get_url()
    {
        return url;
    }
    bool get_size(int &w,int &h)
    {
        bool ret=false;
        if(vcap.isOpened()){
            ret=true;
            w=vcap.get(CV_CAP_PROP_FRAME_WIDTH);
            h=vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
        }
        return ret;
    }
    bool end_with_str(string str,string end_str)
    {
        bool ret=false;
        if(str.size()>end_str.size()){
            string st= str.substr(str.size()-end_str.size(),end_str.size());
            return st==end_str;
        }
        return ret;
    }
    bool get_frame(Mat &frame)
    {
        if(get_pic(frame))
            return true;
        int ret=false;
        frame_lock.lock();
        if(frame_list.size()>0){
            frame_list.front().copyTo(frame);
            frame_list.erase(frame_list.begin());
            ret=true;
        }else{
            ret=false;
        }
        frame_lock.unlock();
        return ret;
    }

    void close_src()
    {
        prt(info,"quiting video %s", url.data());
        quit_flg=true;
        if(src_trd){
            if(src_trd->joinable())
                src_trd->join();
            delete src_trd;
            prt(info,"quiting video thread %s", url.data());
        }
        watch_dog.stop();
        prt(info,"quit video: %s done", url.data());
    }

    inline  bool get_pic(Mat &frame)
    {

        bool ret=false;
        if(is_pic)
        {
            png_frame.copyTo(frame);
            this_thread::sleep_for(chrono::milliseconds(100));
            if(png_frame.cols>0)
                ret=true;
        }
        return ret;

    }

    bool get_frame(Mat &frame, int &timestamp)
    {

        if(get_pic(frame))
            return true;


        bool ret=false;
        frame_lock.lock();
      //prt(info,"timepoint fetch start %ld", get_time_point_ms());
#if 0
        if(frame_list.size()>0){
          //  frame_list.front().copyTo(frame);
            frame_list.back().copyTo(frame);


            //            int start_time=get_ms();
            //  prt(info,"%d,%d",frame.cols,frame.rows);
            //           resize(frame,frame,Size( 640,480),CV_INTER_LINEAR);
            //           int end_time=get_ms();
            //           prt(info,"%d",end_time-start_time);


            frame_list.erase(frame_list.begin());
            timestamp=cur_ms_list.front();
            cur_ms_list.erase(cur_ms_list.begin());
            ret=true;
        }else{
            ret=false;
        }
#else
        if(frame_list.size()>0){
          //  frame_list.front().copyTo(frame);
          //  frame_list.back().copyTo(frame);
            frame_list.front().copyTo(frame);//get old


            //            int start_time=get_ms();
            //  prt(info,"%d,%d",frame.cols,frame.rows);
            //           resize(frame,frame,Size( 640,480),CV_INTER_LINEAR);
            //           int end_time=get_ms();
            //           prt(info,"%d",end_time-start_time);


            frame_list.erase(frame_list.begin());
            timestamp=cur_ms_list.front();
            //cur_ms_list.erase(cur_ms_list.begin());
            ret=true;
        }else{
            ret=false;
        }
#endif
       //  prt(info,"timepoint   fetch end %ld", get_time_point_ms());
        frame_lock.unlock();
        return ret;
    }
    void quit_this()
    {
        lock.lock();
        watch_dog.stop();
        vcap.release();
        quit_flg=true;
        //delete src_trd;
        src_trd->detach();//TODO delete it?
        lock.unlock();

    }

private:
    void run();
    void check_point()
    {
        lock.lock();

       // prt(info,"%s runing , queue len %d",url.data(),queue_length);
        if(vcap.isOpened()){
            prt(info,"%s is runing , frame queue length: %d,frame_rate:%d",url.data(),queue_length,frame_rate);
            //double w= vcap.get(CV_CAP_PROP_POS_FRAMES);
        }else{
            prt(info,"VideoSrc: url: %s is Not running",url.data());
        }
           frame_rate=0;
        lock.unlock();
    }
private:
#ifdef USE_CVCAP
    VideoCapture  vcap;
#else
    FfmpegVideoCapture vcap;
#endif
    Timer1 watch_dog;
    //  PdVideoCapture vcap;
    //  VideoCapture vcap;
    vector <Mat> frame_list;
    vector <int> cur_ms_list;

    int frame_wait_time;
    mutex lock;
    mutex frame_lock;
    int frame_rate;
    string url;
    volatile bool quit_flg;


    thread *src_trd;
    Mat png_frame;
    bool is_pic;
    int try_times;

    bool only_key_frame;
    int queue_length;
    int old_frame_num;
    int old_ntp;

};
#endif
#endif // VIDEOSOURCE_H
