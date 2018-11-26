#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H
#include "videoprocessor.h"

class DummyProcessor:public VideoProcessor
{
    int loopx;
    int loopy;
    DummyProcessorInputData input;
public:
    DummyProcessor(DummyProcessorInputData input_packet):input(input_packet)
    {
        loopx=0;
        loopy=0;
    }
    virtual bool process(Mat img_src,JsonPacket &output_pkt)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
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

#endif // DUMMYPROCESSOR_H
