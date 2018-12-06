#ifndef TEST_H
#define TEST_H

#include "rtmp.h"
#include "encode.h"

#include "videosource.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif
#define TESTFILE

class TestRtmp{
public:
    TestRtmp():encoder(640,480),src("rtsp://192.168.1.95:554/av0_1")
    {
        #ifdef TESTFILE
        if(h264_file.open()){
                prt(info,"err in open");
           // return -1;
        }
#endif
        sender.RTMP264_Connect("rtmp://192.168.1.219/live/test1");
        run();


    }
    ~TestRtmp()
    {

    }
    void run()
    {
        cv::Mat srcImg;
        cv::Mat yuvImg;
        int ts;

      // send_thread=new thread(bind(&TestRtmp::send_rtmp_thread,this));

        while(true){
            if(src.get_frame(srcImg,ts)){
                prt(info,"get a frame");
                cvtColor(srcImg, yuvImg, CV_BGR2YUV_I420);

              //  cvtColor(srcImg, yuvImg, CV_RGB2);
                unsigned char *  pInFrameBuf = (unsigned char *)yuvImg.data;
//                     cv::imshow("1",yuvImg);
//                     cv::waitKey(0);
                unsigned      char *      pInuBuf = (unsigned char *)yuvImg.data + srcImg.cols*srcImg.rows;
                unsigned        char * pInvBuf = (unsigned char *)yuvImg.data +srcImg.cols*srcImg.rows* 5 / 4;

                int ret=encoder.encode(pInFrameBuf);
                if(ret){

                  //  lock.unlock();
                    continue;
                }
                   lock.lock();
                AVPacket &pkt=   encoder.get_pkt();
                //AVPacket p;
                //av_init_packet(&p);
               // p=pkt;

                prt(info,"encode size %d",pkt.size);
                #ifdef TESTFILE
                  h264_file.write(pkt);
#endif
                pkts.push_back(pkt);
                lock.unlock();
                encoder.reset_pkt();

                //sender.

            }else{
                // prt(info,"get a frame fail");
            }
        }
    }

    int read_buffer1(unsigned char *buf, int buf_size ){


        int ret=-1;


        lock.lock();


        if(pkts.size()>10){
            AVPacket &tmp= pkts.front();
            memcpy(buf,tmp.data,tmp.size);
            buf_size=tmp.size;
           // if(buf_size>10000||tmp_loop){
          //      tmp_loop=1;
       //     av_free_packet(&pkts.front());
            pkts.erase(pkts.begin());

//              lock.unlock();
//              return ret;

            ret=buf_size;
            prt(info,"sending sz %d",buf_size);

         //   }

        }
        lock.unlock();
        return ret;
    }
    void send_rtmp_thread()
    {
        sender.read_buffer=bind(&TestRtmp::read_buffer1,this,placeholders::_1,placeholders::_2);
        while(1)
            sender.RTMP264_Send();

    }

private:
    RtmpSender1 sender;
    H264Encoder encoder;
    VideoSource src;
    vector<AVPacket> pkts;
    mutex lock;
    Timer2 t2;
    thread *send_thread;
#ifdef TESTFILE
    H264File h264_file;
#endif
public:
    //static int tmp_loop;
};

#endif // TEST_H

