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
