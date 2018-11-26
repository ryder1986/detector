#ifndef DETECTREGION_H
#define DETECTREGION_H
#include "jsonpacket.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
///#include "videoprocessor.h"
#include "detectregion_data.h"
using namespace std;
using namespace cv;
#if 0
class DummyProcessor:public VideoProcessor
{
    int loopx;
    int loopy;
    DummyProcessorInputData input;
public:
//    DummyProcessor(JsonPacket input_packet):input(input_packet)
//    {
//        loopx=0;
//        loopy=0;
//    }
    DummyProcessor(DummyProcessorInputData input_packet):input(input_packet)
    {
        loopx=0;
        loopy=0;
    }
    virtual bool process(Mat img_src,JsonPacket &output_pkt)
    {

        bool ret=false;
        loopx+=30;
        if(loopx>=img_src.cols)
            loopx=0;

        loopy+=30;
        if(loopy>=img_src.rows)
            loopy=0;
        //        vector<ObjectRect> objs;

        //        ObjectRect r1(loop,11,33,33,"111",99);
        //        objs.push_back(r1.data());
        //        ObjectRect r2(loop,33,33,33,"111",99);
        //        objs.push_back(r2.data());

        vector<VdPoint> ps;
        if(input.Horizon){
            VdPoint p(loopx,10);
            ps.push_back(p);
        }
        if(input.Vertical){
            VdPoint p(10,loopy);
            ps.push_back(p);
        }
        DummyProcessorOutputData d(ps,input.Radii);
        output_pkt=d.data();
        ret=true;
        return  ret;
    }
};

#endif
class DetectRegion : public VdData<DetectRegionInputData>
{
    int tmp;
    VideoProcessor *p;
    Rect detect_rect;
    mutex lock;
public:
    enum OP{
        CHANGE_RECT=1,
        CHANGE_PROCESSOR,
        MODIFY_PROCESSOR
    };
    DetectRegion(DetectRegionInputData pkt);

    ~DetectRegion()
    {
        if(p)
            delete p;
        p=NULL;
    }
public:
    DetectRegionOutputData work(Mat frame);
    void modify(RequestPkt pkt);
    void change_area( vector <VdPoint>poly_vers)
    {
        private_data.ExpectedAreaVers=poly_vers;
    }


private:
    inline void valid_rect(Rect &area,int max_w,int max_h)
    {
        if((area.x+area.width)>max_w)
            area.width=max_w-area.x;
        if((area.y+area.height)>max_h)
            area.height=max_h-area.y;
    }
    inline Rect reshape_2_rect(vector <VdPoint> area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        for(VdPoint pkt: area) {
            int x=pkt.x;
            int y=pkt.y;
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
        x_min=x_min>0?x_min:0;
        y_min=y_min>0?y_min:0;
        x_max=x_max>0?x_max:0;
        y_max=y_max>0?y_max:0;
        return Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
};



#endif // DETECTREGION_H
