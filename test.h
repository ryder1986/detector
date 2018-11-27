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
        prt(info,"start camera right");
        p_src=r.p_src;
        r.p_src=nullptr;
    }
    Camera_Manager(Camera_Manager&)
    {
        prt(info,"start camera copy");
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
