#include "camera_manager.h"
class Cameras_Pri:public JsonObject{

public:
    Cameras_Pri()
    {
    }
    Cameras_Pri(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
public:
    void add_camera(Camera_Pri &&cam)
    {
        CameraData.push_back(cam);
        encode();
    }
//    void add_camera(Camera_Pri cam)
//    {
//        CameraData.push_back(cam);
//        encode();
//    }
    int del_camera(int index)
    {
        if(index<=0||index>CameraData.size())
            return -1;
        CameraData.erase(CameraData.begin()+index-1);
        encode();
        return 0;
    }
private:
    void encode()
    {
        ENCODE_JSONDATA_ARRAY_MEM(CameraData);
    }
    void decode()
    {
        DECODE_JSONDATA_ARRAY_MEM(CameraData);
    }
public:
    vector<Camera_Pri> CameraData;
};
class Cameras_Manager:public VdData<Cameras_Pri>
{
public:
    Cameras_Manager()
    {
    }
    ~Cameras_Manager()
    {
        for(Camera_Manager *p:cameras){
            delete p;
        }
        cameras.clear();
    }

    Cameras_Manager(Cameras_Pri pkt):
        VdData(pkt)
    {
        for(Camera_Pri &cam_config:pkt.CameraData){
            cameras.push_back(new Camera_Manager(cam_config));
        }
    }
private:

private:
    vector <Camera_Manager*> cameras;
};
