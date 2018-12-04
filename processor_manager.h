#ifndef __PROCESSOR__
#define __PROCESSOR__
#include "jsonpacket.h"
#include "opencv2/opencv.hpp"
class VideoProcessor
{
public:
    string alg_rst;

    VideoProcessor()
    {

    }

    virtual ~VideoProcessor()
    {

    }

    virtual bool modify_processor(JsonPacket p)
    {
        return true;
    }

    virtual bool process(cv::Mat img_src,JsonPacket &pkt)=0;

    virtual  string get_rst()
    {
        string ba;
        return ba ;
    }

    virtual void init()
    {
    }

protected:
private:

};
class DummyProcessor_Output:public JsonObject{

public:
    vector<Point_Pri> Points;
    int Radii;
    DummyProcessor_Output()
    {
    }
    DummyProcessor_Output(JsonPacket str):JsonObject(str)
    {
        decode();
    }
    DummyProcessor_Output(vector<Point_Pri> o,int r):Radii(r)
    {
        Points.assign(o.begin(),o.end());
        encode();
    }
    void decode()
    {

        DECODE_JSONDATA_ARRAY_MEM(Points);
        DECODE_INT_MEM(Radii);

    }
    void encode()
    {

        ENCODE_JSONDATA_ARRAY_MEM(Points);
        ENCODE_INT_MEM(Radii);

    }
    template <typename A,typename B,typename C>
    void draw(int offx,int offy,
              A draw_line,
              B draw_circle,C draw_text)
    {
        //        for(Point_Pri p:Points){
        //            draw_circle(Point_Pri(p.x+offx,p.y+offy),Radii,PaintableData::Colour::Red,2);
        //        }
    }

};
class DummyProcessor_Pri:public JsonObject
        //,public PaintableData
{

public:
    bool Horizon;
    bool Vertical;
    int Radii;
    DummyProcessor_Pri(bool h,bool v,int r):Horizon(h),Vertical(v),Radii(r)
    {
        encode();
    }
    DummyProcessor_Pri(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
    DummyProcessor_Pri()
    {

    }
    void decode()
    {

        DECODE_BOOL_MEM(Horizon);
        DECODE_BOOL_MEM(Vertical);
        DECODE_INT_MEM(Radii);

    }
    void encode()
    {

        ENCODE_BOOL_MEM(Horizon);
        ENCODE_BOOL_MEM(Vertical);
        ENCODE_INT_MEM(Radii);

    }
    static DummyProcessor_Pri get_dummy_test_data()
    {
        DummyProcessor_Pri d(true,false,17);
        return d;
    }
};

#endif
