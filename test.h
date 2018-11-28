#ifndef TEST_H
#define TEST_H
#include "tool.h"
#include "jsonpacket.h"
#include "configmanager.h"

class Point_Pri:public JsonData
{
public:
    int x;
    int y;
    Point_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    Point_Pri(int x,int y):x(x),y(y)
    {
        encode();
    }
    Point_Pri()
    {

    }
    void decode()
    {
        DECODE_INT_MEM(x);
        DECODE_INT_MEM(y);
    }
    void encode()
    {
        ENCODE_INT_MEM(x);
        ENCODE_INT_MEM(y);
    }
};
class DummyProcessor_Output:public JsonData{

public:
    vector<VdPoint> Points;
    int Radii;
    DummyProcessor_Output()
    {
    }
    DummyProcessor_Output(JsonPacket str):JsonData(str)
    {
        decode();
    }
    DummyProcessor_Output(vector<VdPoint> o,int r):Radii(r)
    {
        Points.assign(o.begin(),o.end());
        encode();
    }
    void decode()
    {
        try{
            DECODE_JSONDATA_ARRAY_MEM(Points);
            DECODE_INT_MEM(Radii);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_JSONDATA_ARRAY_MEM(Points);
            ENCODE_INT_MEM(Radii);
        }catch(exception e){
            PRT_ENCODE_EXCEPTION
        }
    }
    template <typename A,typename B,typename C>
    void draw(int offx,int offy,
              A draw_line,
              B draw_circle,C draw_text)
    {
        for(VdPoint p:Points){
            draw_circle(VdPoint(p.x+offx,p.y+offy),Radii,PaintableData::Colour::Red,2);
        }
    }

};
class DummyProcessor_Pri:public JsonData,public PaintableData{

public:
    bool Horizon;
    bool Vertical;
    int Radii;
    DummyProcessor_Pri(bool h,bool v,int r):Horizon(h),Vertical(v),Radii(r)
    {
        encode();
    }
    DummyProcessor_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    DummyProcessor_Pri()
    {

    }
    void decode()
    {
        try{
            DECODE_BOOL_MEM(Horizon);
            DECODE_BOOL_MEM(Vertical);
            DECODE_INT_MEM(Radii);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_BOOL_MEM(Horizon);
            ENCODE_BOOL_MEM(Vertical);
            ENCODE_INT_MEM(Radii);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    static DummyProcessor_Pri get_dummy_test_data()
    {
        DummyProcessor_Pri d(true,false,17);
        return d;
    }
};

class DetectRegion_Pri:public JsonData{
public:
    vector<Point_Pri> ExpectedAreaVers;
    JsonPacket ProcessorData;
    string SelectedProcessor;
    DetectRegion_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    DetectRegion_Pri()
    {
    }
    void decode()
    {
        DECODE_STRING_MEM(SelectedProcessor);
        DECODE_PKT_MEM(ProcessorData);
    }
    void encode()
    {
        ENCODE_STRING_MEM(SelectedProcessor);
        DECODE_PKT_MEM(ProcessorData);
    }
};
class CameraData_Pri:public JsonData{

public:
    CameraData_Pri()
    {
    }
    CameraData_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
public:
private:
    void encode()
    {
        ENCODE_JSONDATA_ARRAY_MEM(DetectRegion);
        ENCODE_STRING_MEM(Url);
    }
    void decode()
    {
        DECODE_JSONDATA_ARRAY_MEM(DetectRegion);
        DECODE_STRING_MEM(Url);
    }
public:
    vector<DetectRegion_Pri> DetectRegion;
    string Url;
};
class CameraManagerData_Pri:public JsonData{

public:
    CameraManagerData_Pri()
    {
    }
    CameraManagerData_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
public:
private:
    void encode()
    {
        ENCODE_JSONDATA_ARRAY_MEM(CameraData);
    }
    void decode()
    {
        DECODE_JSONDATA_ARRAY_MEM(CameraData);;
    }
public:
    vector<CameraData_Pri> CameraData;

};
class DeviceConfig_Pri:public JsonData
{
public:
    DeviceConfig_Pri()
    {
    }
    DeviceConfig_Pri(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
public:

private:
    void encode()
    {
        ENCODE_JSONDATA_MEM(DeviceConfig);
    }
    void decode()
    {
        DECODE_JSONDATA_MEM(DeviceConfig);
    }
public:
    CameraManagerData_Pri DeviceConfig;
};








#include "videosource.h"

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

    virtual bool process(Mat img_src,JsonPacket &pkt)=0;

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


class Region_Manager:public VdData<DetectRegion_Pri>
{
public:
    enum OP{
        CHANGE_RECT=1,
        CHANGE_PROCESSOR,
        MODIFY_PROCESSOR
    };
    Region_Manager(DetectRegion_Pri pkt):VdData(pkt)
    {

    }
    ~Region_Manager()
    {

    }

private:


};
class Camera_Manager:public VdData<CameraData_Pri>
{
public:
    Camera_Manager()
    {
    }
    Camera_Manager(CameraData_Pri pkt):
        VdData(pkt)
    {
        prt(info,"start camera %s",pkt.Url.data());
        p_src=new VideoSource(pkt.Url);
    }
    ~Camera_Manager()
    {
        delete p_src;
    }
    Camera_Manager(Camera_Manager&&r)
    {
        p_src=r.p_src;
        r.p_src=nullptr;
    }
 //  Camera_Manager(Camera_Manager&&r)=default;
    Camera_Manager(const Camera_Manager&m)
    {
        p_src=m.p_src;
    }
private:

private:
    VideoSource *p_src;

};

class Cameras_Manager:public VdData<CameraManagerData_Pri>
{
public:
    Cameras_Manager()
    {
    }
    Cameras_Manager(CameraManagerData_Pri pkt):
        VdData(pkt)
    {
        for(CameraData_Pri &cam_config:pkt.CameraData){
           cameras.push_back(Camera_Manager(cam_config));
        }
    }
private:

private:
    vector <Camera_Manager> cameras;

};

class DeviceConfig_Manager:public VdData<DeviceConfig_Pri>
{
public:
    DeviceConfig_Manager()
    {
    }
    DeviceConfig_Manager(DeviceConfig_Pri pkt):
        VdData(pkt),m_cameras(private_data.DeviceConfig)
    {

    }
private:
    Cameras_Manager m_cameras;
};

class Test
{
public:
    Test();
    Test(string str)
    {
        ConfigManager config;
        //prt(info,"%s",config.get_config().str().data());
        DeviceConfig_Pri dev_cfg(config.get_config());
        DeviceConfig_Manager mgr(dev_cfg);
        PAUSE_HERE_FOREVER
        // prt(info,"%s",dev_cfg.data().str().data());
    }
};

#endif // TEST_H
