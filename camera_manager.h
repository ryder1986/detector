#include "region_manager.h"
#include "videosource.h"
class Camera_Output:public JsonObject{
public:
    vector<Region_Output> DetectionResult;
    int Timestamp;
    Camera_Output(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
    Camera_Output()
    {
    }
    Camera_Output(vector <Region_Output> regions,int ts):DetectionResult(regions),Timestamp(ts)
    {
        encode();
    }
    void decode()
    {

        DECODE_INT_MEM(Timestamp);
        DECODE_JSONDATA_ARRAY_MEM(DetectionResult);

    }
    void encode()
    {

        ENCODE_INT_MEM(Timestamp);
        ENCODE_JSONDATA_ARRAY_MEM(DetectionResult);

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

class Camera_Pri:public JsonObject{

public:
    Camera_Pri()
    {
    }
    Camera_Pri(string url,vector<Region_Pri> dr):Url(url),DetectRegion(dr)
    {
        encode();
    }
    Camera_Pri(JsonPacket pkt):JsonObject(pkt)
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
    static Camera_Pri get_test_camera()
    {
        vector<Region_Pri> rs;
        Camera_Pri tmp("rtsp://192.168.1.101:554/av0_1",rs);
        return tmp;
    }
public:
    vector<Region_Pri> DetectRegion;
    string Url;
};


class Camera_Manager:public VdData<Camera_Pri>
{
public:
    Camera_Manager()
    {
    }
    Camera_Manager(Camera_Pri pkt):
        VdData(pkt),frame_rate(0),quit(false)
    {

        prt(info,"start [%p]",this);
        //p_src=new VideoSource(pkt.Url);
        p_src=new VideoSource("rtsp://localhost:80/live/test1");
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
    Camera_Manager(Camera_Manager&&r)=delete;
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

                vector<Region_Output>pkts;


                for(Region_Manager *r:drs){
                    Region_Output ret=r->work(frame);
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
    function <void(Camera_Manager *,Camera_Output)>callback_result=[](Camera_Manager *,Camera_Output){prt(info,"camera call back")};


};
