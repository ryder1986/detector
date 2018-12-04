
#ifndef __DEVICE__
#define __DEVICE__
#include "cameras_manager.h"
#include "jsonpacket.h"
class Device_Pri:public JsonObject
{
public:
    Device_Pri()
    {
    }
    Device_Pri(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
public:

//private:
    void encode()
    {
        ENCODE_JSONDATA_MEM(DeviceConfig);
    }
    void decode()
    {
        DECODE_JSONDATA_MEM(DeviceConfig);
    }
public:
    Cameras_Pri DeviceConfig;
};

class Device_Manager:public VdData<Device_Pri>
{
public:
    Device_Manager()
    {
    }
    Device_Manager(Device_Pri pkt):
        VdData(pkt),m_cameras(private_data.DeviceConfig)
    {

    }
private:
    Cameras_Manager m_cameras;
};
#endif
