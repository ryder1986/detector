#include "camera.h"


void Camera::run_process()
{
    Mat frame;
    int ts;
    while(!quit){
        this_thread::sleep_for(chrono::milliseconds(10));
        if(src->get_frame(frame,ts)){


//            imshow("window",frame);
//            waitKey(0);


            frame_rate++;
#if 0
            for(DetectRegion *r:drs){
                JsonPacket ret=r->work(frame);
                callback_result(this,ret.str());
            }
#endif
            lock.lock();
            vector<DetectRegionOutputData>pkts;
            //   int tmp=0;
            for(DetectRegion *r:drs){
                // prt(info,"region siz %d,now (%d) ",drs.size(),++tmp);

                imwrite("test1.png",frame);
                DetectRegionOutputData ret=r->work(frame);
                pkts.push_back(ret);
            }
            CameraOutputData cod(pkts,ts);
            timestamp=ts;
            //screenshot=frame;

            frame.copyTo(screenshot);



            callback_result(this,cod);
            lock.unlock();
        }
    }
}

bool Camera::modify(RequestPkt req)
{
    int index=req.Index;
    if(index<0||index>drs.size())
        return false;
    prt(info,"handle camera request %d",req.Operation);
    lock.lock();
    switch (req.Operation) {
    case CameraInputData::OP::CHANGE_URL:
    {
        UrlJsonDataRequest u(req.Argument);
        change_source(u.Url);
        private_data.change_url(u.Url);
        break;
    }
    case CameraInputData::OP::INSERT_REGION:
    {
        vector<DetectRegion*>::iterator it=drs.begin();
        DetectRegion *rg=new DetectRegion(req.Argument);
        drs.insert(it+index,rg);
        private_data.insert_region(req.Argument,index);
        break;
    }
    case CameraInputData::OP::DELETE_REGION:
    {
        if(index<=0){
            prt(info,"error index %d",index);
            lock.unlock();
            return false;
        }
        vector<DetectRegion*>::iterator it=drs.begin();
        delete drs[index-1];
        drs.erase(it+index-1);
        private_data.delete_region(index);
        break;
    }
    case CameraInputData::OP::MODIFY_REGION:
    {
         prt(info,"modify region %d",index);
        if(index<=0){
            prt(info,"error index %d",index);
            lock.unlock();
            return false;
        }
        vector<DetectRegion*>::iterator it=drs.begin();
        DetectRegion *rg= drs[index-1];
        // rg->modify(req.Argument.get("ModifyRegion"));
        rg->modify(req.Argument);
        private_data.set_region(rg->get_data().data(),index);
        break;
    }
        default: break;
    }
    lock.unlock();
    return true;
}
