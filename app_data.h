#ifndef APP_DATA_H
#define APP_DATA_H
#include "tool.h"
#include "jsonpacket.h"
#include "camera_data.h"
//class AppInputData:public JsonDataWithTitle{
class AppInputData:public JsonData{
public:
    enum Operation{
        GET_CONFIG=1,
        SET_CONFIG,
        INSERT_CAMERA,
        MODIFY_CAMERA,
        READ_CAMERA,
        DELETE_CAMERA,
        OPEN_CAMERA_DATA,
        CLOSE_CAMERA_DATA,
        HEART_PKT,
        REBOOT
    };
    vector <CameraInputData> CameraData;
    string DeviceName;
    //AppInputData(JsonPacket pkt1):JsonDataWithTitle(pkt1,"DeviceConfig")
    AppInputData(JsonPacket pkt1):JsonData(pkt1)
    {
        try{
            decode();
        }catch(exception e){
            prt(info,"error in decoding AppInputData");
        }
    }
    AppInputData()
    {

    }
    static RequestPkt get_request(int op,int index ,JsonPacket arg)
    {
        switch (op) {
        case AppInputData::GET_CONFIG:

            break;
        case AppInputData::SET_CONFIG:

            break;
        case AppInputData::INSERT_CAMERA:

            break;
        case AppInputData::DELETE_CAMERA:

            break;
        default:
            break;
        }
        return RequestPkt(op, index , arg);
    }
    void set_request(RequestPkt pkt)
    {
        int op=pkt.Operation;
        switch (op) {
        case AppInputData::GET_CONFIG:

            break;
        case AppInputData::SET_CONFIG:
            config=pkt.data();
            decode();
            break;
        default:
            break;
        }
    }
    void modify_camera(CameraInputData data,int index)
    {
        if(index>0&&index<=CameraData.size()){
            CameraData[index-1]=data;
        }else{
            prt(info," cams unchange with index %d", CameraData.size(),index);
        }
        encode();
    }

    void insert_camera(CameraInputData data,int index)
    {
        if(index>=0&&index<=CameraData.size()){
            prt(info," cams  size  %d", CameraData.size());
            vector <CameraInputData>::iterator it=CameraData.begin();
            CameraData.insert(it+index,data);
            prt(info," cams new size  %d", CameraData.size());
        }else{
            prt(info," cams size  %d,unchange with index %d", CameraData.size(),index);
        }
        encode();
    }

    void delete_camera(int index)
    {
        if(index>0&&index<=CameraData.size()){
            vector <CameraInputData>::iterator it=CameraData.begin();
            CameraData.erase(it+index-1);
            prt(info,"del ok,cams new size  %d", CameraData.size());
        }else{
            prt(info," cams size  %d,unchange with index %d", CameraData.size(),index);
        }
        encode();
    }
    void decode()
    {
        try{
            DECODE_STRING_MEM(DeviceName);
            DECODE_JSONDATA_ARRAY_MEM(CameraData);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_STRING_MEM(DeviceName);
            ENCODE_JSONDATA_ARRAY_MEM(CameraData);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
};

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

class AppOutputData:public JsonData{
public:
    int CameraIndex;
    CameraOutputData CameraOutput;
    AppOutputData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }

    AppOutputData(int index,CameraOutputData camout):CameraIndex(index),CameraOutput(camout)
    {
        encode();
    }

    void decode()
    {
        try{
            DECODE_INT_MEM(CameraIndex);
            DECODE_JSONDATA_MEM(CameraOutput);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_INT_MEM(CameraIndex);
            ENCODE_JSONDATA_MEM(CameraOutput);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
};

#endif // APP_DATA_H
