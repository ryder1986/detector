#ifndef TEST_H
#define TEST_H
#include "tool.h"
#include "jsonpacket.h"
#include "configmanager.h"
#define LABEL_PROCESSOR_DUMMY "Dummy"
#define LABEL_PROCESSOR_MVD "Mvd"
class Point_Pri:public JsonObject
{
public:
    int x;
    int y;
    Point_Pri(JsonPacket pkt):JsonObject(pkt)
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
class Vers:public JsonObject{
public:
    vector <Point_Pri>ExpectedAreaVers;
    Vers(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
    Vers(vector <Point_Pri> vs)
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

class DetectRegion_Pri:public JsonObject{
public:
    vector<Point_Pri> ExpectedAreaVers;
    JsonPacket ProcessorData;
    string SelectedProcessor;
    DetectRegion_Pri(JsonPacket pkt):JsonObject(pkt)
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
public:
    void change_rect(const vector<Point_Pri> &vers)
    {

    }
};
class DetectRegion_Output:public JsonObject
{
public:
    VdRect DetectionRect;
    JsonPacket Result;
    DetectRegion_Output(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }

    DetectRegion_Output(JsonPacket rst ,VdRect rct)
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
            B draw_circle,C draw_text,DetectRegion_Pri input_data)
    {
        if(input_data.SelectedProcessor== LABEL_PROCESSOR_DUMMY)
        {
            DummyProcessor_Output data=Result;
            data.draw(DetectionRect.x,DetectionRect.y, draw_line,
                      draw_circle, draw_text);
        }
#if 0
        if(input_data.SelectedProcessor== LABEL_PROCESSOR_MVD)
        {
            MvdProcessorOutputData data=Result;
            MvdProcessorInputData i_data(input_data.ProcessorData);
            data.draw(i_data,DetectionRect.x,DetectionRect.y, draw_line,
                      draw_circle, draw_text);
        }
#endif

    }

};
class CameraData_Pri:public JsonObject{

public:
    CameraData_Pri()
    {
    }
    CameraData_Pri(JsonPacket pkt):JsonObject(pkt)
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

public:
    vector<DetectRegion_Pri> DetectRegion;
    string Url;
};
class CameraManagerData_Pri:public JsonObject{

public:
    CameraManagerData_Pri()
    {
    }
    CameraManagerData_Pri(JsonPacket pkt):JsonObject(pkt)
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
    DetectRegion_Output work(cv::Mat frame)
    {
        lock.lock();
        JsonPacket rst_r;
        valid_rect(detect_rect,frame.cols,frame.rows);
        if(detect_rect.width%2)detect_rect.width--;
        if(detect_rect.height%2)detect_rect.height--;
        cv::Mat tmp=frame(detect_rect);
        if(p &&detect_rect.x>=0&&detect_rect.x<10000
                &&detect_rect.y>=0&&detect_rect.y<10000
                &&detect_rect.width>0&&detect_rect.width<10000
                &&detect_rect.height>0&&detect_rect.height<10000
                &&frame.cols>0&&frame.rows>0
                ){
            p->process(tmp,rst_r);

        }else{
            prt(info,"err arg");
        }
        //  p->process(frame,rst_r);
        VdRect r(detect_rect.x,detect_rect.y,detect_rect.width,detect_rect.height);
        JsonPacket dct_rct=r.data();
        DetectRegion_Output rst(rst_r,dct_rct);
        lock.unlock();
        return rst;
    }


    void modify(RequestPkt pkt)
    {
        prt(info,"request %d",pkt.Operation);
        lock.lock();
        int op=pkt.Operation;
        switch(op){
        case OP::CHANGE_RECT:
        {
            change_rect(Vers(pkt.Argument).ExpectedAreaVers);
            //            AreaVersJsonDataRequest vs(pkt.Argument);
            //            detect_rect=reshape_2_rect(vs.ExpectedAreaVers);
            //            private_data.set_points(vs.ExpectedAreaVers);
            break;
        }
        case OP::CHANGE_PROCESSOR:
        {
            //            if(p){
            //                delete p;
            //                p=NULL;
            //            }
            //            ProcessorDataJsonDataRequest sp(pkt.Argument);
            //            string pro=sp.SelectedProcessor;
            //            if(pro==LABEL_PROCESSOR_DUMMY){
            //                p=new DummyProcessor(sp.ProcessorData);
            //                private_data.set_processor(pro,sp.ProcessorData);
            //            }
            //            if(pro==LABEL_PROCESSOR_MVD)
            //            {
            //                p=new MvdProcessor(sp.ProcessorData);
            //                private_data.set_processor(pro,sp.ProcessorData);
            //            }

            break;
        }
        case OP::MODIFY_PROCESSOR:
        {
            //            p-> modify_processor(pkt.Argument);
            //            private_data.set_processor_data(pkt.Argument);//TODO:fetch data from processor
            break;
        }

defalut:break;
        }
        lock.unlock();
    }

    void change_rect(vector<Point_Pri> points)
    {
        detect_rect=reshape_2_rect(points);
        private_data.change_rect(points);
    }

    static void valid_rect(cv::Rect &area,int max_w,int max_h)
    {
        if((area.x+area.width)>max_w)
            area.width=max_w-area.x;
        if((area.y+area.height)>max_h)
            area.height=max_h-area.y;
    }
    static cv::Rect reshape_2_rect(vector <Point_Pri> area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        for(Point_Pri pkt: area) {
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
        return cv::Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
private:
    cv::Rect detect_rect;
    mutex lock;
    VideoProcessor *p;

};
class Camera_Output:public JsonObject{
public:
    vector<DetectRegion_Output> DetectionResult;
    int Timestamp;
    Camera_Output(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
    Camera_Output()
    {
    }
    Camera_Output(vector <DetectRegion_Output> regions,int ts):DetectionResult(regions),Timestamp(ts)
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
#if 0
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
#endif
};

class Camera_Manager:public VdData<CameraData_Pri>
{
public:
    Camera_Manager()
    {
    }
    Camera_Manager(CameraData_Pri pkt):
        VdData(pkt),frame_rate(0),quit(false)
    {

        prt(info,"start [%p]",this);
        //p_src=new VideoSource(pkt.Url);
        p_src=new VideoSource("rtsp://localhost:5555/test1");
        start();
    }
    ~Camera_Manager()
    {
        quit=true;
        lock.lock();
        if(p_src){
            delete p_src;
        }
        if(work_trd){
            stop();
            //prt(info,"quit");
            delete work_trd;
        }        
        lock.unlock();
        prt(info,"stop [%p]",this);
    }
    Camera_Manager(Camera_Manager&&r)=delete;//
#if 0
    Camera_Manager(Camera_Manager&&r)
    {
        lock.lock();
        p_src=r.p_src;
        r.p_src=nullptr;
        work_trd=r.work_trd;
        r.work_trd=nullptr;

        frame_rate=r.frame_rate;
        //     lock=r.lock;
        vector<Region_Manager*> drs;
        for(Region_Manager *rm:r.drs){
            drs.push_back(rm);
            rm=nullptr;
        }
        quit=r.quit;
        lock.unlock();
        prt(info,"copy done");
    }
#endif
    //  Camera_Manager(Camera_Manager&&r)=default;
    Camera_Manager(const Camera_Manager&m)
    {
        p_src=m.p_src;
    }

    void start()
    {
        work_trd=new thread(bind(&Camera_Manager::run_process,this));
    }

    void stop()
    {
        work_trd->join();
    }
    void run_process()
    {
        cv::Mat frame;
        int ts;
        while(!quit){
            this_thread::sleep_for(chrono::milliseconds(10));
            lock.lock();
            if(p_src->get_frame(frame,ts)){
                // imshow("window",frame);
                // waitKey(0);
                frame_rate++;

                vector<DetectRegion_Output>pkts;


                for(Region_Manager *r:drs){
                    DetectRegion_Output ret=r->work(frame);
                    pkts.push_back(ret);
                }

                Camera_Output co(pkts,123);
                callback_result(this,co);

                //                CameraOutputData cod(pkts,ts);
                //                timestamp=ts;
                //                //screenshot=frame;

                //                frame.copyTo(screenshot);



                //                callback_result(this,cod);

            }
            lock.unlock();
        }
    }
private:

private:
    VideoSource *p_src;
    thread *work_trd;
    int frame_rate;
    mutex lock;
    vector<Region_Manager*> drs;
    bool quit;
    function <void(Camera_Manager *,Camera_Output)>callback_result;


};

class Cameras_Manager:public VdData<CameraManagerData_Pri>
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

    Cameras_Manager(CameraManagerData_Pri pkt):
        VdData(pkt)
    {
        //prt(info,"Cameras_Manager start");

        for(CameraData_Pri &cam_config:pkt.CameraData){
            cameras.push_back(new Camera_Manager(cam_config));
        }
        //prt(info,"Cameras_Manager end");
    }
private:

private:
    vector <Camera_Manager*> cameras;
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

class Test
{
public:
    Test();
    Test(string str)
    {
        try{
            ConfigManager config;
            Device_Pri dev_cfg(config.get_config());
            Device_Manager mgr(dev_cfg);
            PAUSE_HERE_FOREVER
        }catch(exception e){
            prt(info,"exception");
        }
        PAUSE_HERE_FOREVER
    }
};

#endif // TEST_H
