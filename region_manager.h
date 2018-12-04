#include "jsonpacket.h"
#include "processor_manager.h"
#include "opencv2/opencv.hpp"
#define LABEL_PROCESSOR_DUMMY "Dummy"
#define LABEL_PROCESSOR_MVD "Mvd"
class Region_Pri:public JsonObject{
public:
    vector<Point_Pri> ExpectedAreaVers;
    JsonPacket ProcessorData;
    string SelectedProcessor;
    Region_Pri(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }
    Region_Pri()
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
class Region_Output:public JsonObject
{
public:
    VdRect DetectionRect;
    JsonPacket Result;
    Region_Output(JsonPacket pkt):JsonObject(pkt)
    {
        decode();
    }

    Region_Output(JsonPacket rst ,VdRect rct)
    {
        DetectionRect=rct;
        Result=rst;
        encode();
    }

    void decode()
    {
        DECODE_PKT_MEM(DetectionRect);
        DECODE_PKT_MEM(Result);
    }

    void encode()
    {

        ENCODE_JSONDATA_MEM(DetectionRect);
        ENCODE_PKT_MEM(Result);

    }
    template <typename A,typename B,typename C>
    void draw(
            A draw_line,
            B draw_circle,C draw_text,Region_Pri input_data)
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
class Region_Manager:public VdData<Region_Pri>
{
public:
    enum OP{
        CHANGE_RECT=1,
        CHANGE_PROCESSOR,
        MODIFY_PROCESSOR
    };
    Region_Manager(Region_Pri pkt):VdData(pkt)
    {

    }
    ~Region_Manager()
    {

    }
    Region_Output work(cv::Mat frame)
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
        Region_Output rst(rst_r,dct_rct);
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
