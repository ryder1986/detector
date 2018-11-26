#ifndef DETECTREGION_DATA_H
#define DETECTREGION_DATA_H
#include "tool.h"
#include "jsonpacket.h"


#define LABEL_PROCESSOR_DUMMY "Dummy"
#define LABEL_PROCESSOR_MVD "Mvd"

class DummyProcessorOutputData:public JsonData{

public:
    vector<VdPoint> Points;
    int Radii;
    DummyProcessorOutputData()
    {
    }
    DummyProcessorOutputData(JsonPacket str):JsonData(str)
    {
        decode();
    }
    DummyProcessorOutputData(vector<VdPoint> o,int r):Radii(r)
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
class DummyProcessorInputData:public JsonData,public PaintableData{

public:
    bool Horizon;
    bool Vertical;
    int Radii;
    DummyProcessorInputData(bool h,bool v,int r):Horizon(h),Vertical(v),Radii(r)
    {
        encode();
    }
    DummyProcessorInputData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    DummyProcessorInputData()
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
    static DummyProcessorInputData get_dummy_test_data()
    {
        DummyProcessorInputData d(true,false,17);
        return d;
    }
};


//#include "videoprocessor.h"
class AreaVersJsonDataRequest:public JsonData{
public:
    vector <VdPoint>ExpectedAreaVers;
    AreaVersJsonDataRequest(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    AreaVersJsonDataRequest(vector <VdPoint> vs)
    {
        ExpectedAreaVers.assign(vs.begin(),vs.end());
        encode();
    }
    void decode()
    {
        try{
            DECODE_JSONDATA_ARRAY_MEM(ExpectedAreaVers);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_JSONDATA_ARRAY_MEM(ExpectedAreaVers);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }

    }
};

class DetectRegionInputData:public JsonData,public PaintableData
{

public:
    enum OP{
        CHANGE_RECT=1,
        CHANGE_PROCESSOR,
        MODIFY_PROCESSOR
    };
    string SelectedProcessor;
    vector <VdPoint>ExpectedAreaVers;
    JsonPacket ProcessorData;//TODO: dynamic binding
    void *p_processor;
    DetectRegionInputData(JsonPacket pkt):JsonData(pkt),p_processor(NULL)
    {
        decode();
        alloc_processor();
    }
    DetectRegionInputData(JsonPacket pkt,PaintableData pd):JsonData(pkt),PaintableData(pd),p_processor(NULL)
    {
        decode();
        alloc_processor();
    }
    DetectRegionInputData(JsonPacket pdata,string type,vector<VdPoint> vers)
        :ProcessorData(pdata),SelectedProcessor(type),ExpectedAreaVers(vers),p_processor(NULL)
    {
        encode();
        alloc_processor();
    }
    void alloc_processor()
    {
        delete_processor();
        if(SelectedProcessor==LABEL_PROCESSOR_DUMMY){
            p_processor=new DummyProcessorInputData(ProcessorData);
        }
        if(SelectedProcessor==LABEL_PROCESSOR_MVD){
            //p_processor=new MvdProcessorInputData(ProcessorData);
        }
    }
    void delete_processor()
    {
        if(p_processor)
            delete p_processor;
    }

    inline VdRect reshape_2_rect(vector <VdPoint> area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        for(VdPoint pkt: area) {
            int x=pkt.x;
            int y=pkt.y;
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
        x_min=x_min>0?x_min:0;
        y_min=y_min>0?y_min:0;
        x_max=x_max>0?x_max:0;
        y_max=y_max>0?y_max:0;
        return VdRect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
    void set_point(VdPoint p,int index)
    {
        ExpectedAreaVers[index-1]=p;
        encode();
    }

    void set_points(vector <VdPoint> p)
    {
        ExpectedAreaVers=p;
        encode();
    }

    void set_processor_data(JsonPacket p)
    {
        ProcessorData=p;
        encode();
    }

    void set_processor(string p,JsonPacket pd)
    {
        SelectedProcessor=p;
        ProcessorData=pd;
        encode();
    }
    void operator =(DetectRegionInputData dt)
    {
        config=dt.config;
        decode();
    }
    void decode()
    {
        try{
            DECODE_JSONDATA_ARRAY_MEM(ExpectedAreaVers);
            DECODE_PKT_MEM(ProcessorData);
            DECODE_STRING_MEM(SelectedProcessor);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_JSONDATA_ARRAY_MEM(ExpectedAreaVers);
            ENCODE_STRING_MEM(SelectedProcessor);
            ENCODE_PKT_MEM(ProcessorData);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    static DetectRegionInputData get_region_test_data(JsonPacket pkt,string SelectedProcessor)
    {

        vector <VdPoint>ExpectedAreaVers;
        ExpectedAreaVers.push_back(VdPoint(0,0));
        ExpectedAreaVers.push_back(VdPoint(640,0));
        ExpectedAreaVers.push_back(VdPoint(640,480));
        ExpectedAreaVers.push_back(VdPoint(0,480));

        DetectRegionInputData rd(pkt,SelectedProcessor,ExpectedAreaVers);
        return rd;
    }

    static DetectRegionInputData get_region_test_data()
    {

        DummyProcessorInputData di=DummyProcessorInputData::get_dummy_test_data();
        //   DetectRegionInputData dri=DetectRegionInputData::get_region_test_data(di.data(),LABEL_PROCESSOR_DUMMY);

        vector <VdPoint>ExpectedAreaVers;
        ExpectedAreaVers.push_back(VdPoint(0,0));
        ExpectedAreaVers.push_back(VdPoint(640,0));
        ExpectedAreaVers.push_back(VdPoint(640,480));
        ExpectedAreaVers.push_back(VdPoint(0,480));

        DetectRegionInputData rd(di.data(),LABEL_PROCESSOR_DUMMY,ExpectedAreaVers);
        return rd;
    }


    static RequestPkt get_request(int op,int index ,JsonPacket arg)
    {
        switch (op) {
        case CHANGE_RECT:

            break;
        default:
            break;
        }
        return RequestPkt(op, index , arg);
    }

    virtual bool press(VdPoint pnt_ori)
    {


        return false;
    }
    enum CMD{
        SET_MVD=0,
        SET_DUMMY,
        CMD_COUNT
    };

     void change_processor_2_dummy()
    {
        prt(info,"change processor to dummy");
    }
    void change_processor_2_mvd()
    {

    }

    template <typename A>
    bool right_press(VdPoint pnt,A exec_menu)
    {
        if((point_index=p_on_v(ExpectedAreaVers,pnt))){
            return true;
        }
        if(p_on_vl(ExpectedAreaVers,pnt)){

            return true;
        }
        return true;
    }
    virtual bool move(VdPoint pnt_ori)
    {
        if(seizing){
            switch (event_type) {
            case Event::MoveVer:
                ExpectedAreaVers[point_index-1]=pnt_ori;
                break;
            case Event::MoveAll:
            {
                int offx=pnt_ori.x-ori_pnt.x;
                int offy=pnt_ori.y-ori_pnt.y;
                ori_pnt=pnt_ori;
                int i=0;
                int sz=ExpectedAreaVers.size();
                for(i=0;i<sz;i++){
                    ExpectedAreaVers[i]=VdPoint(ExpectedAreaVers[i].x+offx,ExpectedAreaVers[i].y+offy);
                }
                break;
            }
            default:
                break;
            }
            encode();
            return true;

        }

        if(SelectedProcessor== LABEL_PROCESSOR_MVD)
        {

        }
        return true;
    }
    virtual bool double_click(VdPoint pnt)
    {
        return true;
    }
    virtual void release()
    {
        if(seizing){
            AreaVersJsonDataRequest req(ExpectedAreaVers);
            DetectRegionInputData::get_request_pkt(DetectRegionInputData::CHANGE_RECT,0,req.data());
            release_event();
        }
    }
    bool release(RequestPkt &req)
    {
        if(seizing){
            AreaVersJsonDataRequest req1(ExpectedAreaVers);
            req=DetectRegionInputData::get_request_pkt(DetectRegionInputData::CHANGE_RECT,0,req1.data());
            release_event();
            return true;
        }

        if(SelectedProcessor== LABEL_PROCESSOR_MVD)
        {
            return true;

            //      MvdProcessorInputData mid(ProcessorData,paintable_data);

            //    mid.release();
            //            draw_text(LABEL_PROCESSOR_MVD,VdPoint(100,200),100,PaintableData::Blue,30);
            //            VdRect r= reshape_2_rect(ExpectedAreaVers);
            //            MvdProcessorInputData data(ProcessorData);
            //            data.draw(r.x,r.y,draw_line, draw_circle,draw_text);
            //       MvdProcessorInputData data=Result;
            //     dat
            //            data.draw(DetectionRect.x,DetectionRect.y, draw_line,
            //                       draw_circle, draw_text);
        }
        return false;
    }

    template <typename A,typename B,typename C>
    void draw(
            A draw_line,
            B draw_circle,C draw_text)
    {
        for(int i=1;i<ExpectedAreaVers.size();i++){
            if(!seizing)
                draw_line(ExpectedAreaVers[i-1],ExpectedAreaVers[i],PaintableData::Colour::Blue,2);
            else
                draw_line(ExpectedAreaVers[i-1],ExpectedAreaVers[i],PaintableData::Colour::Red,2);
        }
        if(!seizing)
            draw_line(ExpectedAreaVers.front(),ExpectedAreaVers.back(),PaintableData::Colour::Blue,2);
        else
            draw_line(ExpectedAreaVers.front(),ExpectedAreaVers.back(),PaintableData::Colour::Red,2);


        if(SelectedProcessor== LABEL_PROCESSOR_DUMMY)
        {
            draw_text(LABEL_PROCESSOR_DUMMY,VdPoint(100,200),100,PaintableData::Blue,30);
            VdRect r= reshape_2_rect(ExpectedAreaVers);
            // DummyProcessorInputData data=Result;
            //            data.draw(DetectionRect.x,DetectionRect.y, draw_line,
            //                       draw_circle, draw_text);
        }
        if(SelectedProcessor== LABEL_PROCESSOR_MVD)
        {

        }

    }

};
class ProcessorDataJsonDataRequest:public JsonData{
public:
    string SelectedProcessor;
    JsonPacket ProcessorData;
    ProcessorDataJsonDataRequest(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    ProcessorDataJsonDataRequest(string pro,JsonPacket d)
    {
        ProcessorData=d;
        SelectedProcessor=pro;
        encode();
    }
    void decode()
    {
        try{
            DECODE_STRING_MEM(SelectedProcessor);
            DECODE_PKT_MEM(ProcessorData);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    void encode()
    {
        try{
            ENCODE_STRING_MEM(SelectedProcessor);
            ENCODE_PKT_MEM(ProcessorData);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
};
class DetectRegionOutputData:public JsonData
{
public:
    VdRect DetectionRect;
    JsonPacket Result;
    DetectRegionOutputData(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }

    DetectRegionOutputData(JsonPacket rst ,VdRect rct)
    {
        DetectionRect=rct;
        Result=rst;
        encode();
    }

    void decode()
    {
        try{
            DECODE_PKT_MEM(DetectionRect);
            DECODE_PKT_MEM(Result);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }

    void encode()
    {
        try{
            ENCODE_JSONDATA_MEM(DetectionRect);
            ENCODE_PKT_MEM(Result);
        }catch(exception e){
            PRT_DECODE_EXCEPTION
        }
    }
    template <typename A,typename B,typename C>
    void draw(
            A draw_line,
            B draw_circle,C draw_text,DetectRegionInputData input_data)
    {
        if(input_data.SelectedProcessor== LABEL_PROCESSOR_DUMMY)
        {
            DummyProcessorOutputData data=Result;
            data.draw(DetectionRect.x,DetectionRect.y, draw_line,
                      draw_circle, draw_text);
        }
        if(input_data.SelectedProcessor== LABEL_PROCESSOR_MVD)
        {

        }

    }

};


#endif // DETECTREGION_DATA_H
