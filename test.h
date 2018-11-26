#ifndef TEST_H
#define TEST_H
#include "tool.h"
#include "jsonpacket.h"
#include "configmanager.h"


class DeviceConfigData:public JsonData{
public:
    AppInputData DeviceConfig;
    DeviceConfigData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    DeviceConfigData(AppInputData appdata):DeviceConfig(appdata)
    {
        encode();
    }
    DeviceConfigData()
    {
        //decode();
    }
    void decode()
    {
        try{
            DECODE_JSONDATA_MEM(DeviceConfig);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_JSONDATA_MEM(DeviceConfig);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
};
class CameraData_Pri:public JsonData{

public:
    CameraData_Pri()
    {

    }
private:
    void encode()
    {

    }
    void decode()
    {

    }
private:
    AppInputData DeviceConfig;
};
class DeviceConfig_Pri:public JsonData{

public:
    DeviceConfig_Pri()
    {

    }
private:

    void encode()
    {

    }
    void decode()
    {

    }
private:
    AppInputData DeviceConfig;
};

class Test
{
public:
    Test();
    Test(string str)
    {
        ConfigManager  config;
    }
};

#endif // TEST_H
