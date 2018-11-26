#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H
#include "tool.h"
#include "jsonpacket.h"
#include "detectregion_data.h"
class CameraInputData:public JsonData,PaintableData
{
public:
    enum OP{
        CHANGE_URL=1,
        INSERT_REGION,
        DELETE_REGION,
        MODIFY_REGION
    };
    string Url;
    vector <DetectRegionInputData >DetectRegion;
    static inline string get_default_url()
    {
        return "rtsp://192.168.1.95:554/av0_1";
    }

    CameraInputData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    CameraInputData()
    {
    }
    CameraInputData(vector <DetectRegionInputData> regions,string url):DetectRegion(regions),Url(url)
    {
        encode();
    }
    void set_region(JsonPacket data,int index)
    {
        DetectRegion[index-1]=data;
        encode();
    }

    void insert_region(JsonPacket data,int index)
    {
        vector <DetectRegionInputData >::iterator begin=DetectRegion.begin();
        DetectRegion.insert(begin+index,data);
        encode();
    }
    void change_url(string u)
    {
        Url=u;
        encode();
    }
    void change_url(JsonPacket url)
    {
        Url=url.get("Url").to_string();
        encode();
    }
    void delete_region(int index)
    {
        vector <DetectRegionInputData >::iterator begin=DetectRegion.begin();
        DetectRegion.erase(begin+index-1);
        encode();
    }
    void decode()
    {
        try{
            DECODE_STRING_MEM(Url);
            DECODE_JSONDATA_ARRAY_MEM(DetectRegion);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_STRING_MEM(Url);
            ENCODE_JSONDATA_ARRAY_MEM(DetectRegion);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    virtual bool press(VdPoint pnt)
    {
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            int ret=dt.press(pnt);
            if(ret)
                return ret;
        }
        return false;
    }
    virtual bool press_right_button(VdPoint pnt)
    {
        //        for(int i=0;i<DetectRegion.size();i++){
        //           DetectRegionInputData &dt= DetectRegion[i];
        //           dt.press(pnt);
        //        }
        return true;
    }
    JsonPacket get_url_pkt()
    {
        JsonPacket p;
        p.add("Url",CameraInputData::get_default_url());
        return p;
    }

    virtual bool right_press(VdPoint pnt,vector<RequestPkt> &pkts,vector<string> &text)
    {
        vector<RequestPkt> pkts_tmp;
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            if(dt.right_press(pnt,pkts_tmp,text)){
                for(RequestPkt p:pkts_tmp){
                    pkts.push_back(get_request_pkt(CameraInputData::MODIFY_REGION,i+1,\
                                                   p.data()));
                }
                RequestPkt  req_del_region=get_request_pkt(CameraInputData::DELETE_REGION,i+1,\
                                                           JsonPacket());
                pkts.push_back(req_del_region);
                text.push_back("del this region");
                return true;
            }
        }

        // req_mod=get_request_pkt(CameraInputData::MODIFY_REGION,i+1,req1.data());
        RequestPkt  req_append_region=get_request_pkt(CameraInputData::INSERT_REGION,DetectRegion.size(),\
                                                      DetectRegionInputData::get_region_test_data().data());
        pkts.push_back(req_append_region);
        text.push_back("append region");
        RequestPkt req_reset_url=get_request_pkt(CameraInputData::CHANGE_URL,0,get_url_pkt());
        pkts.push_back(req_reset_url);
        text.push_back("reset url");
        return true;
    }
    virtual bool move(VdPoint pnt)
    {
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            dt.move(pnt);
            encode();
        }
        return true;
    }
    virtual bool double_click(VdPoint pnt)
    {
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            dt.double_click(pnt);
        }
        return true;
    }
    virtual void release()
    {
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            dt.release();
        }

    }
    void modify_rect(RequestPkt pkt)
    {

    }
    bool release(RequestPkt &req)
    {
        RequestPkt req1;
        for(int i=0;i<DetectRegion.size();i++){
            DetectRegionInputData &dt= DetectRegion[i];
            if(dt.release(req1)){
                req=get_request_pkt(CameraInputData::MODIFY_REGION,i+1,req1.data());
                return true;
            }
        }
        return false;
    }

    template <typename A,typename B,typename C>
    void draw(
            A draw_line,
            B draw_circle,C draw_text)
    {
        int sz=DetectRegion.size();
        for(int i=0;i<sz;i++){
            DetectRegionInputData &dt= DetectRegion[i];
            dt.draw(draw_line,draw_circle,draw_text);
        }
    }
    static CameraInputData get_camera_test_data(vector <DetectRegionInputData> regions,string url)
    {
        CameraInputData rd(regions,url);
        return rd;
    }
};

class CameraOutputData:public JsonData
{
public:
    vector<DetectRegionOutputData> DetectionResult;
    int Timestamp;
    CameraOutputData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    CameraOutputData()
    {
    }
    CameraOutputData(vector <DetectRegionOutputData> regions,int ts):DetectionResult(regions),Timestamp(ts)
    {
        encode();
    }
    void decode()
    {
        try{
            DECODE_INT_MEM(Timestamp);
            DECODE_JSONDATA_ARRAY_MEM(DetectionResult);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_INT_MEM(Timestamp);
            ENCODE_JSONDATA_ARRAY_MEM(DetectionResult);
             }
        catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }

    template <typename A,typename B,typename C>
    void draw(CameraInputData data,
              A draw_line,
              B draw_circle,C draw_text)
    {
        int sz=DetectionResult.size();
        for(int i=0;i<sz;i++){
            DetectRegionOutputData &dt= DetectionResult[i];
            if(i>=data.DetectRegion.size()){
                prt(info,"region %d outof range ",i);
                continue;
            }
            dt.draw(draw_line,draw_circle,draw_text,data.DetectRegion[i]);
            draw_text(data.Url,VdPoint(200,200),100,PaintableData::Blue,30);
#if 0
            char buf[20];
            sprintf(buf,"data  ts:%d",Timestamp);
            string str(buf);
            draw_text(str,VdPoint(200,300),100,PaintableData::Blue,30);
#endif
        }
    }
};
class UrlJsonDataRequest:public JsonData
{
public:
    string Url;
    UrlJsonDataRequest(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    UrlJsonDataRequest(string url)
    {
        Url=url;
        encode();
    }
    void decode()
    {
        try{
            DECODE_STRING_MEM(Url);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_STRING_MEM(Url);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
};

#endif // CAMERA_DATA_H
