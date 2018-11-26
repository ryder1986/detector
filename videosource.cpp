#include "videosource.h"
#include <thread>
#include <functional>

VideoSource::VideoSource(string path):watch_dog(bind(&VideoSource::check_point,this)),
    frame_rate(0),is_pic(false),src_trd(NULL)
{
    only_key_frame=false;
    try_times=0;
    url=path;
    watch_dog.start(1000);
    queue_length=3;
#if 0
    vcap.open(path);
#endif
    prt(info,"start %s result: %d",path.data(),vcap.isOpened());

    quit_flg=false;
    //  thread(bind(&VideoSource::run,this)).detach();
    // _start_async(bind(&VideoSource::run,this));

    if(end_with_str(url,"png")){
        imread(url).copyTo(png_frame);
        prt(info,"read png");
        is_pic=true;
    }else
        src_trd=new thread(bind(&VideoSource::run,this));
}

VideoSource::VideoSource(string path,bool only_keyframe):watch_dog(bind(&VideoSource::check_point,this)),
    frame_rate(0),vcap(path),is_pic(false),src_trd(NULL)
{
    only_key_frame=only_keyframe;

    try_times=0;
    //  Timer1 t1(bind(&VideoSource::check_point,this));
    watch_dog.start(1000);
    prt(info,"%s",path.data());
    url=path;
    quit_flg=false;
    queue_length=3;
    if(end_with_str(url,"png")){
        imread(url).copyTo(png_frame);
        prt(info,"read png");
        is_pic=true;
    }else
        src_trd=new thread(bind(&VideoSource::run,this));
}
VideoSource::~VideoSource()
{
   quit_this();
//    auto qf=bind(&VideoSource::quit_this,this);
//    Timer2 t2;
//    t2.AsyncWait(0,qf);


#if 0
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
//#else
    lock.lock();
    vcap.release();
    watch_dog.stop();
    quit_flg=true;
    delete src_trd;
    lock.unlock();
    //src_trd->detach();
    //new thread(bind(&VideoSource::close_src,this));

#endif

}
void VideoSource::run()
{
    //     vcap.open(path);
    lock.lock();
#ifdef USE_CVCAP
    vcap=VideoCapture(url);
#else
    vcap=FfmpegVideoCapture(url);
#endif
    //  this_thread::sleep_for(chrono::milliseconds(1000));
    if(!vcap.isOpened()){
        prt(info,"fail to open %s", url.data());
    }else{
        prt(info,"ok to open %s", url.data());
    }

    if(1){
        frame_wait_time=10;
    }else{
        frame_wait_time=40;
    }

    lock.unlock();
    Mat frame;
    int flag_retry=0;
    while(true){

        lock.lock();
         //prt(info,"timepoint  frame start %ld", get_time_point_ms());
        if(quit_flg){
            break;
        }

        if( vcap.isOpened()){
            flag_retry=0;
            frame.release();
            //prt(info,"timepoint  read start %ld", get_time_point_ms());

            bool rt= vcap.read(frame);
          //  prt(info,"timepoint  read end %ld", get_time_point_ms());

            if(!rt){
                // cout<<url.data()<<" get frame error!"<<endl;
                prt(info,"get frame fail,restart video capture %s", url.data());
                vcap.release();
#ifdef USE_CVCAP
                vcap=VideoCapture(url);
#else
                vcap=FfmpegVideoCapture(url);
#endif
                //   vcap=VideoCapture( url.data());
                //vcap=PdVideoCapture( url.data());
            }
            if(frame.cols==0&&++try_times==100){
                vcap.release();
                prt(info,"%s get frame error,retrying ... ", url.data());
                continue;
            }else{
                //prt(info,"timepoint  frame start1 %ld", get_time_point_ms());

                long int ts=vcap.get(CV_CAP_PROP_POS_MSEC);
                long int fs=vcap.get(CV_CAP_PROP_POS_FRAMES);
                int dis=fs-old_frame_num;
                int ms=ts/100;
                if(dis>0&&dis<1000){
                      ms+=40*dis;
                }
                   if(old_ntp!=ts)
                     old_frame_num=fs;
                old_ntp=ts;
                //    int ts=vcap.get(CV_CAP_PROP_POS_AVI_RATIO);

                //     int ts=vcap.get(CV_CAP_PROP_POS_FRAMES);;
                //    int ts=vcap.get(CV_CAP_PROP_FRAME_COUNT);

                //  int ts=vcap.get(CV_CAP_PROP_POS_AVI_RATIO);
                //                double ts1=vcap.get(CV_CAP_PROP_POS_FRAMES);
                //                double ts2=vcap.get(CV_CAP_PROP_FRAME_COUNT);
                //                double ts3=vcap.get(CV_CAP_PROP_POS_MSEC);
               // long  ts=cvGetTickCount();
                // prt(info,"%ld ", tc);
              //  prt(info,"timestamp  %lu ", ts);
                //prt(info,"timestamp  %lu ", fs);
                frame_rate++;
                //this_thread::sleep_for(chrono::milliseconds( 40));


                if(!(frame_rate%30))
                {
                    //prt(info,"running %s",url.data());
                }
              //  prt(info,"timepoint  frame start2 %ld", get_time_point_ms());

                frame_lock.lock();
                if(frame.rows>0&&frame.cols>0){
                    frame_list.push_back(frame);
                    cur_ms_list.push_back(ms);
                    //cur_ms_list.push_back(ts/1000+fs*40);
                    while(frame_list.size()>queue_length&&queue_length){
                        frame_list.erase(frame_list.begin());
                        cur_ms_list.erase(cur_ms_list.begin());
                    }
                }
                frame_lock.unlock();
               // prt(info,"timepoint  frame start3 %ld", get_time_point_ms());

                if(frame_wait_time)
                    this_thread::sleep_for(chrono::milliseconds( frame_wait_time));
            }
        }else{
#if 1
            this_thread::sleep_for(chrono::seconds(1));
#else
            if(flag_retry++<10){
                //if(flag_retry++<10){
                this_thread::sleep_for(chrono::milliseconds(100));
            }else{
                this_thread::sleep_for(chrono::seconds(1));
            }
#endif
#ifdef USE_CVCAP
            vcap=VideoCapture(url);
#else
            vcap=FfmpegVideoCapture(url);
#endif
            prt(info,"open url err:%s",url.data());
        }
        lock.unlock();
         //prt(info,"timepoint  frame done %ld", get_time_point_ms());
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    prt(info,"thread is quiting");
    lock.lock();
    if( vcap.isOpened())
        vcap.release();
    lock.unlock();
}

