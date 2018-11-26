#ifndef APP_H
#define APP_H

#include "configmanager.h"
#include "camera.h"
#include "app_data.h"
#include "server.h"
#include "tool.h"
#include <mysql/mysql.h>
#include <list>

#define MAX_CAM_NUM 1000
#define MAX_LANE_NUM 10000
//#define COUNT_SECONDS 15
#define COUNT_SECONDS 3600
#define RECORD_SIZE 100
class DatabaseInstance
{
public:
    static DatabaseInstance &get_instance()
    {
        static DatabaseInstance ins;
        return ins;
    }
    bool connect()
    {
        if(!mysql_real_connect(sql,host.data(),user.data(),passwd.data(),NULL,3306,0,0)){
            sql=NULL;
            const char *err= mysql_error(sql);
            prt(info,"connect database error: %s" ,err);
            mysql_close(sql);
            return false;
        }else{
            prt(info,"connect database %s ok",db.data());
            int ret=  mysql_select_db(sql,db.data());
            prt(info,"ok use %d",ret);
            return true;
        }
    }

    bool connect(string host,string user,string passwd,string db)
    {
        if(!mysql_real_connect(sql,host.data(),user.data(),passwd.data(),NULL,3306,0,0)){
            sql=NULL;
            const char *err= mysql_error(sql);
            prt(info,"connect database error: %s" ,err);
            mysql_close(sql);
            return false;
        }else{
            prt(info,"connect database %s ok",db.data());
            int ret=  mysql_select_db(sql,db.data());
            prt(info,"ok use %d",ret);
            return true;
        }
    }
    void print_result()
    {
    }
    void print_table()
    {
        if(!query_flag){
            prt(info,"eror:can not print table  because query failed");
            return ;
        }
#if 1
        //   MYSQL_FIELD *field;
        if(!resid)
            return;
        while((field = mysql_fetch_field(resid)))
        {
            printf(" %s ", field->name);
        }
#endif

        cout<<endl;
        // query("select * from  table1");
        //  resid = mysql_store_result(sql);
        MYSQL_ROW row;
        unsigned int num_fields;
        unsigned int i;

        num_fields = mysql_num_fields(resid);
        // return ;
        while ((row = mysql_fetch_row(resid)))
        {
            unsigned long *lengths;
            lengths = mysql_fetch_lengths(resid);
            for(i = 0; i < num_fields; i++)
            {
                //  field = mysql_fetch_field(resid);
                //  printf("field   %p\n", field);
                // printf("field name %s\n", field->name);
                printf("[%.*s] ", (int) lengths[i],
                       row[i] ? row[i] : "NULL");
            }
            printf("\n");
        }
    }
    void query(char *statement)
    {
        lock.lock();

        //mysql_free_result(resid );
        int mysqlret = mysql_real_query(sql,statement,(unsigned int)strlen(statement));
        if(mysqlret!=0)
        {
            query_flag=false;
            //       abort();
            //  assert(false);
            const char *err_str=  mysql_error(sql);
            prt(info,"error: %s" , err_str);
        }else{
            query_flag=true;
            resid = mysql_store_result(sql);
        }
        lock.unlock();
    }
    ~DatabaseInstance()
    {
        mysql_close(sql);
    }
private:
    DatabaseInstance(char *host,char *user,char*passwd,char *db):
        host(host),user(user),passwd(passwd),db(db),sql(NULL),query_flag(false),need_connect(true)
    {
        sql=mysql_init(NULL);
    }

    DatabaseInstance()
    {
        sql=mysql_init(NULL);
    }
    string host;
    string user;
    string passwd;
    string db;
    MYSQL *sql;
    MYSQL_RES *resid;
    MYSQL_FIELD *field;
    bool query_flag;
    bool need_connect;
    mutex lock;
    // MYSQL_ROW row_str_list;
};

class DestClient{
public:

    DestClient(string i):ip(i)
    {

    }

    string get_ip()
    {
        return ip;
    }

private:
    string ip;
};
class App:public VdData<AppInputData>
{
    struct vs_table{
        string record_id;
        string sql_time;
        string period;
        string a_id;
        string avenue;
        string cameraid;

        string laneid;
        string vihicle_sum;
        string ahead_sum;
        string rear_sum;

        string truck_sum;
        string bus_sum;
        string car_sum;
        string motor_sum;
        string bicycle_sum;

        string average_speed;
        string average_time;
        string average_space;
        string average_desity;
        string average_occupy;
        string state;
    };
public:
    App();
    App(ConfigManager *p);
    ~App();
    void start()
    {
    }
    void test_sql()
    {
        //  DatabaseInstance &ins= DatabaseInstance::get_instance();

        DatabaseInstance &ins=get_database();

        while(1) {

            ins.query(
                        "INSERT INTO VS                  ( `RecordID`, `SST`,\
                        `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `LaneID`, \
                        `VSum`, `VPSum`, `VNSum`, `VTSum`, `VBSum`, `VCSum`, `VMSum`,  \
                        `VKSum`, `ASpeed`, `ATime`, `ASpace`, `AVM`, `AOccupy`,`RState`) \
                    VALUES('999','2018-11-13-09:31:00','5','66','66','66',\
                           '66','3','3','0','0','0','3','0','0','45','46','36','37','2','1');"

            );

            SLEEP_HERE_MS(1000);
        }
    }
    void handle_json_cmd(Session *src, char *data, int len)
    {
         string ret_str;
             bool process_ret;
        prt(info,"json cmd %s",data);
        // RequestPkt event(json_buf);
                        ReplyPkt ret_pkt;
                        client_tmp_ip=src->ip();

                                 RequestPkt event(string(data,len));
                        process_ret=process_event(event,ret_pkt);
                        ret_str=ret_pkt.data().str();
                        src->send(ret_str.data(),ret_str.length());
    }
private:
    bool process_event(RequestPkt e,ReplyPkt &r);
    void process_client_cmd(Session *clt,char *data,int len);
    void client_data_request(Session *clt, char *data, int len);
    void process_camera_data(Camera *clt,CameraOutputData);
    Camera* new_cam(CameraInputData data)
    {
        return new Camera(data,bind(&App::process_camera_data,
                                    this,placeholders::_1,
                                    placeholders::_2));
    }
    void save_data()
    {
        DeviceConfigData data(private_data);
        p_cm->set_config(data.data().str());//get
    }
    JsonPacket load_data()
    {
        DeviceConfigData data( p_cm->get_config());
        private_data=data.data();
    }


    void restart_all()
    {
        stop_cams();
        start_cams();
    }
    void start_cams()
    {
        for(CameraInputData p:private_data.CameraData){
            cms.push_back(new_cam( p));
        }
    }
    void stop_cams()
    {
        for(Camera *c:cms){
            delete c;
        }
        cms.clear();
    }
    bool add_camera(int index,CameraInputData data)//after who ?  0~size
    {
        bool ret=false;
        if(0<=index&&index<=cms.size()){
            Camera *c=new_cam( (data));
            vector<Camera*>::iterator it=cms.begin();
            cms.insert(it+index,c);
            private_data.insert_camera(data,index);
            ret=true;
        }
        return ret;
    }
    bool del_camera(int index)//delete who ? 1~size
    {
        if(1<=index&&index<=cms.size()){
            Camera *cm=cms[index-1];

            vector<Camera*>::iterator it=cms.begin();
            cms.erase(it+index-1);
            private_data.delete_camera(index);

#if 0
            delete cms[index-1];
#else
            thread([this,index,cm](){ delete cm;}).detach();
#endif
            return true;
        }
        return false;
    }
    bool mod_camera(int index,RequestPkt pkt)//delete who ? 1~size
    {
        bool ret=false;
        if(1<=index&&index<=cms.size()){
            vector<Camera*>::iterator it=cms.begin();
            Camera *c=cms[index-1];
            if(c->modify(pkt)){
                private_data.modify_camera(c->get_data(),index);
                ret=true;
            }
        }
        return ret;
    }
    void check_point()
    {
        //  return ;

        //        while(1){
        //            this_thread::sleep_for(chrono::seconds(1));
        //        }
        int count=dir_count("/ftphome/pic");
        //  prt(info,"/ftphome/pic pics count %d---->",count);
        // prt(info,"server camera count %d---->",cms.size());
        int left=1000;
        if(count>left){
            delete_dir_files("/ftphome/pic",count,left);
            delete_dir_files("/ftphome/video",count,left);

        }
    }


    //////////db/////////


    DatabaseInstance  &  get_database()
    {
        DatabaseInstance &ins=DatabaseInstance::get_instance();
        string user="root";
        string passwd="root";
        string db="AIPD";
        string host="localhost";
        string fn;
        fn.append("/ftphome/pic/");
        prt(info,"database start");
        if(sql_need_connect){
            ins.connect(host,user,passwd,db);
            sql_need_connect=false;
            prt(info,"database starting..");
        }
        prt(info,"database done");
        return ins;
    }
    void get_tis_str(char *buf,char *time, int type)
    {

        //  "INSERT INTO TIS ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `RegionID`, `TEType`, `TEPAddr`, `TEVAddr`)\
        //   VALUES  ( '3', '2018-03-07 17:46:24', '60', '32', '大学城', '12', '12', '21', 'pic_path', 'video_path');"

        sprintf(buf," INSERT INTO TIS ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `RegionID`, `TEType`, `TEPAddr`, `TEVAddr`)\
                VALUES  ( '3', '%s', '60', '32', '大学城', '12', '12', '%d', 'pic_path', 'video_path')",time,type);
    }
    void single_insert(EventRegionObjectOutput data,Mat frame)
    {
        //if(mvddata.DriveAwayData.size()>0){
        if(1){
            prt(info,"get incident");
            DatabaseInstance &ins=DatabaseInstance::get_instance();
            string user="root";
            string passwd="root";
            string db="AIPD";
            string host="localhost";
            string fn;
            fn.append("/ftphome/pic/");

            stringstream  stream;
            stream<<get_time_point_ms();
            fn.append(stream.str());
            fn.append(".png");
            imwrite(fn,frame);
            if(sql_need_connect){
                ins.connect(host,user,passwd,db);
                sql_need_connect=false;
            }
            static char bf[1000];
            memset(bf,0,1000);
            get_tis_str(bf,get_sql_time(),1);
            prt(info,"start qurey");
            ins.query(bf);
            prt(info,"stop qurey");
        }
    }
    string insert_pic(Mat frame)
    {
        if(1){
            prt(info,"get incident");

            string user="root";
            string passwd="root";
            string db="AIPD";
            string host="localhost";
            string fn;
            fn.append("/ftphome/pic/");

            stringstream  stream;
            stream<<get_time_point_ms();
            fn.append(stream.str());
            fn.append(".png");
            imwrite(fn,frame);
            prt(info,"get pic");
            stream<<".png";
            return stream.str();


        }
    }


    void insert_picture(Mat frame,vector <VdPoint> outline,int type,vector <VdPoint> region,string pic_path)
    {
        string str;
        // prt(info,"inserting pic1");
        switch(type){
        case EventRegion::OVER_SPEED:
            str.append("OVER_SPEED");
            break;
        case EventRegion::REVERSE_DRIVE:
            str.append("REVERSE_DRIVE");
            break;
        case EventRegion::STOP_INVALID:
            str.append("STOP_INVALID");
            break;
        case EventRegion::NO_PEDESTRIANTION:
            str.append("NO_PEDESTRIANTION");
            break;
        case EventRegion::DRIVE_AWAY:
            str.append("DRIVE_AWAY");
            break;
        case EventRegion::CONGESTION:
            str.append("CONGESTION");
            break;
        case EventRegion::AbANDON_OBJECT:
            str.append("AbANDON_OBJECT");
        case EventRegion::NON_MOTOR:
            str.append("NON_MOTOR");
            break;
        default:break;

        }
        // return "1";
        // prt(info,"inserting pic2");
        putText(frame, str, Point(100,130),CV_FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,255),3,8);
        VdRect rct= vers_2_rect(region);
        VdPoint offset(rct.x,rct.y);
        //            prt(info," start record rect incident");
        //           for(VdPoint p:outline){
        //            prt(info," () %d %d)",p.x,p.y);
        //           }
        //            prt(info," end record rect incident ");
        //           prt(info,"offset %d %d",rct.x,rct.y);
        if(outline.size()==4){
            for(int i=0;i<outline.size()-1;i++){
                VdPoint start=add_point_offset(outline[i],offset);
                VdPoint end=add_point_offset(outline[i+1],offset);
                line(frame, Point(start.x,start.y),Point( end.x,end.y), Scalar(255, 255 ,0), 3, 8, 0 );
            }
            VdPoint start=add_point_offset(outline.front(),offset);
            VdPoint end=add_point_offset(outline.back(),offset);
            line(frame, Point(start.x,start.y),Point( end.x,end.y), Scalar(255, 255 ,0), 3, 8, 0 );

        }
        //        string user="root";
        //        string passwd="root";
        //        string db="AIPD";
        //        string host="localhost";
        // prt(info,"inserting pic3");
        imwrite(pic_path,frame);
        // prt(info,"inserting pic4");
    }

    int insert_video(int cam_index,string path)
    {
        if(buffer_frames[cam_index-1].size()==0)
        {
            return 0;
        }
        //prt(info,"inserting video start");
        Mat fst=buffer_frames[cam_index-1].front();
        //  cv::VideoWriter recVid(path, cv:: VideoWriter::fourcc('X', 'V', 'I', 'D'), 15,  cv::Size(fst.cols, fst.rows));
        //   cv::VideoWriter recVid(path, cv:: VideoWriter::fourcc('M', 'J', 'P', 'G'), 15,  cv::Size(fst.cols, fst.rows));

        //   cv::VideoWriter recVid(path, cv:: VideoWriter::fourcc('D', 'I', 'V', 'X'), 15,  cv::Size(fst.cols, fst.rows));
        //  cv::VideoWriter recVid(path, cv:: VideoWriter::fourcc('X', '2', '6', '4'), 15,  cv::Size(fst.cols, fst.rows));
        cv::VideoWriter recVid(path, cv:: VideoWriter::fourcc('X', '2', '6', '4'), 10,  cv::Size(fst.cols, fst.rows));
        if (!recVid.isOpened())
        {
            prt(info,"err in open video file");
            return -1;
        }
        //prt(info,"start insert");
        vector <Mat> mts;

        for(Mat mt:buffer_frames[cam_index-1]){
            Mat m;
            mt.copyTo(m);
            mts.push_back(m);
        }

        for(Mat mt:mts){
            //prt(info,"frame size ( %d %d )",mt.cols,mt.rows);
            recVid<<mt;
            //prt(info,"frame write done");
        }
        recVid.release();

        //prt(info,"inserting video done");
        return 1;
    }
    void get_names(string &pic_name,string &video_name)
    {
        stringstream  stream;
        stream<<get_time_point_ms();
        pic_name.append(stream.str());
        video_name.append(stream.str());
        pic_name.append(".png");
        //video_name.append(".avi");
        video_name.append(".mp4");
    }
    string insert_pic_ex(Mat frame,vector <VdPoint> outline,int type,vector <VdPoint> region)
    {
        string str;

        switch(type){
        case EventRegion::OVER_SPEED:
            str.append("OVER_SPEED");
            break;
        case EventRegion::REVERSE_DRIVE:
            str.append("REVERSE_DRIVE");
            break;
        case EventRegion::STOP_INVALID:
            str.append("STOP_INVALID");
            break;
        case EventRegion::NO_PEDESTRIANTION:
            str.append("NO_PEDESTRIANTION");
            break;
        case EventRegion::DRIVE_AWAY:
            str.append("DRIVE_AWAY");
            break;
        case EventRegion::CONGESTION:
            str.append("CONGESTION");
            break;
        case EventRegion::AbANDON_OBJECT:
            str.append("AbANDON_OBJECT");
        case EventRegion::NON_MOTOR:
            str.append("NON_MOTOR");
            break;
        default:break;

        }

        putText(frame, str, Point(100,130),CV_FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,255),3,8);
        VdRect rct= vers_2_rect(region);
        VdPoint offset(rct.x,rct.y);
        //            prt(info," start record rect incident");
        //           for(VdPoint p:outline){
        //            prt(info," () %d %d)",p.x,p.y);
        //           }
        //            prt(info," end record rect incident ");
        //           prt(info,"offset %d %d",rct.x,rct.y);
        if(outline.size()==4){
            for(int i=0;i<outline.size()-1;i++){
                VdPoint start=add_point_offset(outline[i],offset);
                VdPoint end=add_point_offset(outline[i+1],offset);
                line(frame, Point(start.x,start.y),Point( end.x,end.y), Scalar(255, 255 ,0), 3, 8, 0 );
            }
            VdPoint start=add_point_offset(outline.front(),offset);
            VdPoint end=add_point_offset(outline.back(),offset);
            line(frame, Point(start.x,start.y),Point( end.x,end.y), Scalar(255, 255 ,0), 3, 8, 0 );

        }
        //        string user="root";
        //        string passwd="root";
        //        string db="AIPD";
        //        string host="localhost";
        string fn;
        fn.append("/ftphome/pic/");

        stringstream  stream;
        stream<<get_time_point_ms();
        fn.append(stream.str());
        fn.append(".png");
        imwrite(fn,frame);
        //prt(info,"get pic");
        stream<<".png";
        return stream.str();
    }
    void store_frame(Mat mt,int index)
    {
        if(buffer_frames[index-1].size()>RECORD_SIZE){
            buffer_frames[index-1].erase(buffer_frames[index-1].begin());
        }
        buffer_frames[index-1].push_back(mt);
    }
    typedef struct lane_out_data{
        int frames;
        int vehicle_count;

        int direction;

        int old_timepoint;
        int timepoint;

        int old_exist;
        int exist;

        int time_sum;

        int time_in;
        int time_out;
        int time_exist;

        int begin_flow;
        int end_flow;

        int car_flow_end;
        int truck_flow_end;
        int bus_flow_end;

        int moto_end;
        int bicycle_end;


        int car_flow_begin;
        int truck_flow_begin;
        int bus_flow_begin;
        int moto_begin;
        int bicycle_begin;

        int speed_sum;
    }m_lane_out_data;
    vector <m_lane_out_data> lod[MAX_CAM_NUM];
    void handle_frame(MvdProcessorOutputData data,int index)
    {
       prt(info,"handle a frame");
        //data.LaneOutputData;
        vector <LaneOutputJsonData> &laneout=data.LaneOutputData;
        vector <lane_out_data> &laneout_count=lod[index-1];
        if(laneout.size()>0){
            if(laneout_count.size()==0){
             //   prt(info,"handle a frame,first");

                m_lane_out_data first;
                first.frames=0;
                first.old_timepoint=get_time_point_ms()/1000;
                first.timepoint=first.old_timepoint;

                first.old_exist=true;
                first.exist=true;

                first.time_sum=0;
                first.vehicle_count=0;


                for(int i=0;i<laneout.size();i++){
                    LaneOutputJsonData &tmp_data=laneout[i];
                    first.begin_flow=tmp_data.VehicleFlow;



                    first.bus_flow_begin=tmp_data.BusFlow;
                    first.truck_flow_begin=tmp_data.TruckFlow;
                    first.car_flow_begin=tmp_data.CarFlow;
                    first.bicycle_begin=tmp_data.BicycleFlow;
                    first.moto_begin=tmp_data.MotorbikeFlow;

                    laneout_count.push_back(first);
                }
            }else{
               //  prt(info,"handle a frame ,normal");
                if(laneout.size()==laneout_count.size()){
                    for(int i=0;i<laneout.size();i++){
                        LaneOutputJsonData &tmp_data=laneout[i];
                        m_lane_out_data &tmp_data_count=laneout_count[i];
                        //update timepoint
                        tmp_data_count.timepoint=get_time_point_ms()/1000;

                        tmp_data_count.exist=tmp_data.NearCarExist;
                        if(tmp_data_count.exist&&!(tmp_data_count.old_exist)){
                            if(tmp_data_count.vehicle_count>=1){


                                tmp_data_count.timepoint=get_time_point_ms()/1000;
                                prt(info,"################enter timepoint   %d ",tmp_data_count.timepoint);
                                tmp_data_count.time_in= tmp_data_count.timepoint;
                                tmp_data_count.time_sum+=(tmp_data_count.timepoint-tmp_data_count.old_timepoint);
                                prt(info,"################time dis   %d ",(tmp_data_count.timepoint-tmp_data_count.old_timepoint));

                                tmp_data_count.old_timepoint=tmp_data_count.timepoint;

                                   }
                            tmp_data_count.vehicle_count++;
                            tmp_data_count.speed_sum+=tmp_data.VehicleSpeed;
                            prt(info,"################speed  %d ",tmp_data.VehicleSpeed);
                        }
                        if(!(tmp_data_count.exist)&&(tmp_data_count.old_exist)){
                            if(tmp_data_count.vehicle_count>=1){
                                tmp_data_count.time_out=get_time_point_ms()/1000;
                                if(tmp_data_count.time_out>tmp_data_count.time_in&&tmp_data_count.time_in){
                                    tmp_data_count.time_exist+=(tmp_data_count.time_out-tmp_data_count.time_in);
                                 prt(info,"#########in out#######   %d  %d ",tmp_data_count.time_out,tmp_data_count.time_in);
                                }
                            }
                        }

                        tmp_data_count.old_exist=tmp_data_count.exist;
                        tmp_data_count.direction=tmp_data.LaneDirection;


                        tmp_data_count.end_flow=tmp_data.VehicleFlow;
                        tmp_data_count.bus_flow_end=tmp_data.BusFlow;
                        tmp_data_count.truck_flow_end=tmp_data.TruckFlow;
                        tmp_data_count.car_flow_end=tmp_data.CarFlow;
                        tmp_data_count.bicycle_end=tmp_data.BicycleFlow;
                        tmp_data_count.moto_end=tmp_data.MotorbikeFlow;


                    }


                }

            }
        }

    }
    inline void reset_laneout()
    {
        for(int i=0;i<MAX_CAM_NUM;i++)  {

            lod[i].clear();
        }
    }
    void handle_result(CameraOutputData data,int index,Mat frame)
    {
        if(index<1||index>cms.size())
        {
            prt(info,"err index %d inserting database,now %d cameras avliable",index,cms.size());
        }
        // store_frame(frame,index);
        //Timer2 t2;t2.AsyncWait(0,bind(&App::store_frame,this,placeholders::_1),frame);
        // prt(info,"handle camera %d",index);
        CameraInputData input=cms[index-1]->get_data();
        //prt(info,"handle camera %d",index);
        for(int i=0;i<data.DetectionResult.size();i++){
            DetectRegionOutputData d=data.DetectionResult[i];
            DetectRegionInputData  id= input.DetectRegion[i];
            JsonPacket rst=d.Result;
            //   prt(info," %s ",id.SelectedProcessor.data());
            if(id.SelectedProcessor==LABEL_PROCESSOR_MVD){
                MvdProcessorOutputData mvddata(rst);
                //  prt(info,"handle camera %d",index);
                flow_lock.lock();
                //outputs[index-1].push_back(mvddata);
                handle_frame(mvddata,index);
                //                if(!mvddata.LaneOutputData.size()){
                //                    prt(info,"size 0, %s",mvddata.data().str().data());
                //                }
                flow_lock.unlock();
                // prt(info,"handle camera %d",index);
                if(mvddata.NewEventFlag)
                    for(EventRegionObjectOutput eo:mvddata.EventObjects){
                        //  prt(info,"handle camera %d",index);
                        if(eo.EventID){
                            //   prt(info," inserting event %d begin ",eo.Type);
                            string picname;
                            string videoname;
                            string picpath("/ftphome/pic/");
                            string videopath("/ftphome/video/");
                            get_names(picname,videoname);
                            picpath.append(picname);
                            videopath.append(videoname);
                            // Timer2 t2;
                            Mat  frame_tmp;
                            frame.copyTo(frame_tmp);
                            //  prt(info,"inserting video");
                            // t2.AsyncWait(0,bind(&App::insert_video,this,placeholders::_1,placeholders::_2),index,videopath);
                            insert_video(index,videopath);
                            //  prt(info,"inserting pic");
                            insert_picture(frame_tmp,eo.Vers,eo.Type,id.ExpectedAreaVers,picpath);
                            //  prt(info,"inserting tis");
                            database_insert_tis(get_sql_time(),eo.Type,picpath.data(),videopath.data());
                            //  prt(info,"inserting event done");

                        }
                    }
            }
        }
        //SLEEP_HERE_MS(1000);
    }

#if 1
    inline bool exist_event(EventRegionObjectOutput event,int index)
    {

    }
    inline bool exist_in_last(EventRegionObjectOutput event,int index)
    {
        for(EventRegionObjectOutput e:last_events[index-1]){
            if(e.EventID==event.EventID&&e.Type==event.Type){

                return true;
            }else{
                prt(info,"new event-> id %d (old %d), type %d(old %d)",e.EventID,event.EventID,e.Type,event.Type);
            }
        }
        return false;
    }
#else
    inline bool exist_in_last(EventRegionObjectOutput event,int index)
    {
        for(EventRegionObjectOutput e:last_events[index-1]){
            if(e.EventID==event.EventID&&e.Type==event.Type){


            }else{
                prt(info,"new event-> id %d (old %d), type %d(old %d)",e.EventID,event.EventID,e.Type,event.Type);
                return true;
            }
        }
        return true;
    }
#endif
    void database_insert_tis(const char *sql_time,int type,const char * pic_path,const char * video_path)
    {
        prt(info,"event %d",type);
        DatabaseInstance &ins=DatabaseInstance::get_instance();

        string user="root";
        string passwd="root";
        string db="AIPD";
        string host="localhost";
        if(sql_need_connect){
            ins.connect(host,user,passwd,db);
            sql_need_connect=false;
        }
        //        ins.query("INSERT INTO TIS \
        //        ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `RegionID`, `TEType`, `TEPAddr`, `TEVAddr`)\
        //           VALUES\
        //        ( '3', '2018-03-07 17:46:24', '60', '32', 'abc', '12', '12', '21', 'pic_path', 'video_path');");

        char buf[1000];memset(buf,0,1000);
#if 1
        sprintf(buf,"INSERT INTO TIS \
                ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `RegionID`, `TEType`, `TEPAddr`, `TEVAddr`)\
                VALUES\
                ( '3', '%s', '60', '32', 'abcde', '12', '12', '%d', '%s', '%s');",sql_time,type,pic_path,video_path);
        #else

        sprintf(buf,"INSERT INTO TIS \
                ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `RegionID`, `TEType`, `TEPAddr`, `TEVAddr`)\
                VALUES\
                ( '3', '%s', '60', '32', 'abcde', '12', '12', '%d', '%s', 'video_path');",get_sql_time(),1,"5678");
        #endif
                ins.query(buf);
    }
    inline void do_report( MvdProcessorInputData &input,
                           vector< MvdProcessorOutputData> & outputs,
                           vector <vs_table> &tables,
                           string sql_time)
    {
        vs_table table;
        table.record_id=int_2_string(999);
        table.sql_time=sql_time;
        table.period=int_2_string(COUNT_SECONDS);
        table.a_id="66";
        table.avenue="66";
        table.cameraid="66";


        int lanesize=input.LaneData.size();

        MvdProcessorOutputData begin=outputs.front();
        MvdProcessorOutputData end=outputs.back();

        int vihicle_sum=0;
        int ahead_sum=0;
        int rear_sum=0;

        int truck_sum=0;
        int bus_sum=0;
        int car_sum=0;
        int motor_sum=0;
        int bicycle_sum=0;


        int speed_sum=0;
        int time_sum=0;
        int space_sum=0;
        int desity_sum=0;
        int occupy_sum=0;

        for(int j=0;j<lanesize;j++){

            table.laneid="66";

            table.vihicle_sum=int_2_string(0);
            table.ahead_sum=int_2_string(0);;
            table.rear_sum=int_2_string(0);;
            table.truck_sum=int_2_string(0);;
            table.bus_sum=int_2_string(0);;
            table.car_sum=int_2_string(0);;
            table.motor_sum=int_2_string(0);;
            table.bicycle_sum=int_2_string(0);;
            table.average_speed=int_2_string(0);;
            table.average_time=int_2_string(0);;
            table.average_space=int_2_string(0);;
            table.average_desity=int_2_string(0);;
            table.average_occupy=int_2_string(0);;
            table.state=int_2_string(0);;



            if(begin.LaneOutputData.size()<=j)
                continue;
            if(end.LaneOutputData.size()<=j)
                continue;
            LaneOutputJsonData tmp_begin=begin.LaneOutputData[j];
            LaneOutputJsonData tmp_end=end.LaneOutputData[j];
            vihicle_sum=tmp_end.VehicleFlow-tmp_begin.VehicleFlow;
            if(!vihicle_sum){
                tables.push_back(table);
                continue;
            }
            //            ahead_sum=vihicle_sum;
            //            rear_sum=0;

            truck_sum=tmp_end.TruckFlow-tmp_begin.TruckFlow;
            bus_sum=tmp_end.BusFlow-tmp_begin.BusFlow;
            car_sum=tmp_end.CarFlow-tmp_begin.CarFlow;

            motor_sum=tmp_end.MotorbikeFlow-tmp_begin.MotorbikeFlow;
            bicycle_sum=tmp_end.BicycleFlow-tmp_begin.BicycleFlow;

            int exist=0;
            for(int i=0;i<outputs.size();i++){
                MvdProcessorOutputData o= outputs[i];
                LaneOutputJsonData l=o.LaneOutputData[j];
                speed_sum+=l.VehicleSpeed;
                time_sum+=l.VehicleHeadtime;
                space_sum+=l.VehicleDensity;
                desity_sum+=l.VehicleDensity;
                if(l.NearCarExist){
                    exist++;
                }
                if(l.LaneDirection){
                    ahead_sum=vihicle_sum;
                    rear_sum=0;
                }else{
                    ahead_sum=0;
                    rear_sum=vihicle_sum;
                }
            }

            table.vihicle_sum=int_2_string(vihicle_sum);
            table.ahead_sum=int_2_string(ahead_sum);
            table.rear_sum=int_2_string(rear_sum);
            table.truck_sum=int_2_string(truck_sum);
            table.bus_sum=int_2_string(bus_sum);
            table.car_sum=int_2_string(car_sum);
            table.motor_sum=int_2_string(motor_sum);
            table.bicycle_sum=int_2_string(bicycle_sum);

            table.average_speed=int_2_string(speed_sum/outputs.size());
            table.average_time=int_2_string(time_sum/outputs.size());
            table.average_space=int_2_string(space_sum/outputs.size());

            table.average_desity=int_2_string(desity_sum/outputs.size());
            table.average_occupy=int_2_string(exist*100/outputs.size());
            table.state=int_2_string(1);

            tables.push_back(table);
        }


    }

    inline void do_report1( MvdProcessorInputData &input,
                            //               vector< MvdProcessorOutputData> & outputs,
                            vector <vs_table> &tables,
                            string sql_time,
                            vector <m_lane_out_data> &lane_out_data)
    {
        vs_table table;
        table.record_id=int_2_string(999);
        table.sql_time=sql_time;
        table.period=int_2_string(COUNT_SECONDS);
        table.a_id="66";
        table.avenue="66";
        table.cameraid="66";
        prt(info,"do report1 ");

        int lanesize=input.LaneData.size();

        //        MvdProcessorOutputData begin=outputs.front();
        //        MvdProcessorOutputData end=outputs.back();

        int vihicle_sum=0;
        int ahead_sum=0;
        int rear_sum=0;

        int truck_sum=0;
        int bus_sum=0;
        int car_sum=0;
        int motor_sum=0;
        int bicycle_sum=0;


        int speed_sum=0;
        int time_sum=0;
        int space_sum=0;
        int desity_sum=0;
        int occupy_sum=0;

        for(int j=0;j<lanesize;j++){
            m_lane_out_data &ld=lane_out_data[j];
            LaneDataJsonData &lane=input.LaneData[j];

            prt(info,"do report1 : flow %d , timesum %d",ld.vehicle_count,ld.time_sum);



            table.laneid=int_2_string((j+1));

            table.vihicle_sum=int_2_string(0);
            table.ahead_sum=int_2_string(0);;
            table.rear_sum=int_2_string(0);;
            table.truck_sum=int_2_string(0);;
            table.bus_sum=int_2_string(0);;
            table.car_sum=int_2_string(0);;
            table.motor_sum=int_2_string(0);;
            table.bicycle_sum=int_2_string(0);;
            table.average_speed=int_2_string(0);;
            table.average_time=int_2_string(0);;
            table.average_space=int_2_string(0);;
            table.average_desity=int_2_string(0);;
            table.average_occupy=int_2_string(0);;
            table.state=int_2_string(0);;


            if(lane_out_data.size()<=j)
                continue;
            //            if(begin.LaneOutputData.size()<=j)
            //                continue;
            //            if(end.LaneOutputData.size()<=j)
            //                continue;
            //            LaneOutputJsonData tmp_begin=begin.LaneOutputData[j];
            //            LaneOutputJsonData tmp_end=end.LaneOutputData[j];
            //            vihicle_sum=tmp_end.VehicleFlow-tmp_begin.VehicleFlow;
            //            if(!vihicle_sum){
            //                tables.push_back(table);
            //                continue;
            //            }
            //            ahead_sum=vihicle_sum;
            //            rear_sum=0;

            //            truck_sum=tmp_end.TruckFlow-tmp_begin.TruckFlow;
            //            bus_sum=tmp_end.BusFlow-tmp_begin.BusFlow;
            //            car_sum=tmp_end.CarFlow-tmp_begin.CarFlow;

            //            motor_sum=tmp_end.MotorbikeFlow-tmp_begin.MotorbikeFlow;
            //            bicycle_sum=tmp_end.BicycleFlow-tmp_begin.BicycleFlow;

            //int exist=0;
            //            for(int i=0;i<outputs.size();i++){
            //                MvdProcessorOutputData o= outputs[i];
            //                LaneOutputJsonData l=o.LaneOutputData[j];
            //                speed_sum+=l.VehicleSpeed;
            //                time_sum+=l.VehicleHeadtime;
            //                space_sum+=l.VehicleDensity;
            //                desity_sum+=l.VehicleDensity;
            //                if(l.NearCarExist){
            //                    exist++;
            //                }
            //                if(l.LaneDirection){
            //                    ahead_sum=vihicle_sum;
            //                    rear_sum=0;
            //                }else{
            //                    ahead_sum=0;
            //                    rear_sum=vihicle_sum;
            //                }
            //            }

            truck_sum=ld.truck_flow_end-ld.truck_flow_begin;
            bus_sum=ld.bus_flow_end-ld.bus_flow_begin;
            car_sum=ld.car_flow_end-ld.car_flow_begin;

            motor_sum=ld.moto_end-ld.moto_begin;
            bicycle_sum=ld.bicycle_end-ld.bicycle_begin;

            vihicle_sum=ld.vehicle_count;
            if(ld.direction){
                ahead_sum=vihicle_sum;
                rear_sum=0;
            }else{
                ahead_sum=0;
                rear_sum=vihicle_sum;
            }
            time_sum=ld.time_sum;
            speed_sum=ld.speed_sum;



            table.vihicle_sum=int_2_string(ld.vehicle_count);
            table.ahead_sum=int_2_string(ahead_sum);
            table.rear_sum=int_2_string(rear_sum);
            table.truck_sum=int_2_string(truck_sum);
            table.bus_sum=int_2_string(bus_sum);
            table.car_sum=int_2_string(car_sum);
            table.motor_sum=int_2_string(motor_sum);
            table.bicycle_sum=int_2_string(bicycle_sum);


            if(ld.vehicle_count>1){
                table.average_time=int_2_string(time_sum/(ld.vehicle_count-1));
            }

            if(ld.vehicle_count>0){
                table.average_speed=int_2_string(speed_sum/ld.vehicle_count);
                table.average_space=int_2_string((speed_sum/ld.vehicle_count*1000)/3600*(time_sum/ld.vehicle_count));
            }

            //            table.average_speed=int_2_string(speed_sum/outputs.size());
            //            table.average_time=int_2_string(time_sum/outputs.size());
            //            table.average_space=int_2_string(space_sum/outputs.size());

            //            table.average_desity=int_2_string(desity_sum/outputs.size());
            //            table.average_occupy=int_2_string(exist*100/outputs.size());

            table.state=int_2_string(1);
            tables.push_back(table);
        }

        reset_laneout();
    }


    inline void do_report2( MvdProcessorInputData &input,
                            //               vector< MvdProcessorOutputData> & outputs,
                            vector <vs_table> &tables,
                            string sql_time,
                            vector <m_lane_out_data> &lane_out_data,string url)
    {
        vs_table table;
        table.record_id=int_2_string(999);
        table.sql_time=sql_time;
        table.period=int_2_string(COUNT_SECONDS);
        table.a_id="66";
        table.avenue=url;
        table.cameraid=url;
        prt(info,"do report1 ");

        int lanesize=input.LaneData.size();

        //        MvdProcessorOutputData begin=outputs.front();
        //        MvdProcessorOutputData end=outputs.back();

   //     prt(info,"error ");
        int vihicle_sum=0;
        int ahead_sum=0;
        int rear_sum=0;

        int truck_sum=0;
        int bus_sum=0;
        int car_sum=0;
        int motor_sum=0;
        int bicycle_sum=0;


        int speed_sum=0;
        int time_sum=0;
        int space_sum=0;
        int desity_sum=0;
        int occupy_sum=0;
     //   prt(info,"error ");

        for(int j=0;j<lanesize;j++){
          //  prt(info,"error ");
            if(lane_out_data.size()!=lanesize){
                  prt(info,"size %d ",lane_out_data.size());
                  continue;
            }
                m_lane_out_data &ld=lane_out_data[j];

          //  LaneDataJsonData &lane=input.LaneData[j];

            prt(info,"do report1 : flow %d , timesum %d",ld.vehicle_count,ld.time_sum);



            table.laneid=int_2_string((j+1));

            table.vihicle_sum=int_2_string(0);
            table.ahead_sum=int_2_string(0);;
            table.rear_sum=int_2_string(0);;
            table.truck_sum=int_2_string(0);;
            table.bus_sum=int_2_string(0);;
            table.car_sum=int_2_string(0);;
            table.motor_sum=int_2_string(0);;
            table.bicycle_sum=int_2_string(0);;
            table.average_speed=int_2_string(0);;
            table.average_time=int_2_string(0);;
            table.average_space=int_2_string(0);;
            table.average_desity=int_2_string(0);;
            table.average_occupy=int_2_string(0);;
            table.state=int_2_string(0);;


            if(lane_out_data.size()<=j)
                continue;
            //            if(begin.LaneOutputData.size()<=j)
            //                continue;
            //            if(end.LaneOutputData.size()<=j)
            //                continue;
            //            LaneOutputJsonData tmp_begin=begin.LaneOutputData[j];
            //            LaneOutputJsonData tmp_end=end.LaneOutputData[j];
            //            vihicle_sum=tmp_end.VehicleFlow-tmp_begin.VehicleFlow;
            //            if(!vihicle_sum){
            //                tables.push_back(table);
            //                continue;
            //            }
            //            ahead_sum=vihicle_sum;
            //            rear_sum=0;

            //            truck_sum=tmp_end.TruckFlow-tmp_begin.TruckFlow;
            //            bus_sum=tmp_end.BusFlow-tmp_begin.BusFlow;
            //            car_sum=tmp_end.CarFlow-tmp_begin.CarFlow;

            //            motor_sum=tmp_end.MotorbikeFlow-tmp_begin.MotorbikeFlow;
            //            bicycle_sum=tmp_end.BicycleFlow-tmp_begin.BicycleFlow;

            //int exist=0;
            //            for(int i=0;i<outputs.size();i++){
            //                MvdProcessorOutputData o= outputs[i];
            //                LaneOutputJsonData l=o.LaneOutputData[j];
            //                speed_sum+=l.VehicleSpeed;
            //                time_sum+=l.VehicleHeadtime;
            //                space_sum+=l.VehicleDensity;
            //                desity_sum+=l.VehicleDensity;
            //                if(l.NearCarExist){
            //                    exist++;
            //                }
            //                if(l.LaneDirection){
            //                    ahead_sum=vihicle_sum;
            //                    rear_sum=0;
            //                }else{
            //                    ahead_sum=0;
            //                    rear_sum=vihicle_sum;
            //                }
            //            }

            truck_sum=(ld.truck_flow_end-ld.truck_flow_begin);
            if(truck_sum<0){truck_sum=0;prt(info,"warning , get invid flow");}
            bus_sum=ld.bus_flow_end-ld.bus_flow_begin;
               if(bus_sum<0){bus_sum=0;prt(info,"warning , get invid flow");}
            car_sum=ld.car_flow_end-ld.car_flow_begin;
            if(car_sum<0){car_sum=0;prt(info,"warning , get invid flow");}

            motor_sum=ld.moto_end-ld.moto_begin;
            if(motor_sum<0){motor_sum=0;prt(info,"warning , get invid flow");}

            bicycle_sum=ld.bicycle_end-ld.bicycle_begin;
            if(bicycle_sum<0){bicycle_sum=0;prt(info,"warning , get invid flow");}

          //  vihicle_sum=ld.vehicle_count;
            vihicle_sum=  ld.end_flow-ld.begin_flow-motor_sum-bicycle_sum;

            occupy_sum=ld.time_exist*100/COUNT_SECONDS;

            prt(info,"ocupytime %d ",occupy_sum);
            if(occupy_sum<0)occupy_sum=0;
            if(occupy_sum>100)occupy_sum=occupy_sum%100;

            if(vihicle_sum<0){vihicle_sum=0;prt(info,"warning , get invid flow");}

           // if(ld.direction){
            if(true){
                ahead_sum=vihicle_sum;
                rear_sum=0;
            }else{
                ahead_sum=0;
                rear_sum=vihicle_sum;
            }
            time_sum=ld.time_sum;
            speed_sum=ld.speed_sum;



            table.vihicle_sum=int_2_string(vihicle_sum);
            table.ahead_sum=int_2_string(ahead_sum);
            table.rear_sum=int_2_string(rear_sum);
            table.truck_sum=int_2_string(truck_sum);
            table.bus_sum=int_2_string(bus_sum);
            table.car_sum=int_2_string(car_sum);
            table.motor_sum=int_2_string(motor_sum);
            table.bicycle_sum=int_2_string(bicycle_sum);
            table.average_occupy=int_2_string(occupy_sum);

         //   prt(info,"error ");

            if(vihicle_sum>1){
                table.average_time=int_2_string(time_sum/vihicle_sum);
            }

            if(vihicle_sum>0){
                table.average_speed=int_2_string(speed_sum/vihicle_sum);
                table.average_space=int_2_string((speed_sum/vihicle_sum*1000)/3600*(time_sum/vihicle_sum));
                prt(info,"count %d, time %d  speed %d",vihicle_sum,time_sum,speed_sum);
            }

            //            table.average_speed=int_2_string(speed_sum/outputs.size());
            //            table.average_time=int_2_string(time_sum/outputs.size());
            //            table.average_space=int_2_string(space_sum/outputs.size());

            //            table.average_desity=int_2_string(desity_sum/outputs.size());
            //            table.average_occupy=int_2_string(exist*100/outputs.size());

            table.state=int_2_string(1);
            tables.push_back(table);
        }

        reset_laneout();
    }

    void flow_thread()
    {
//        return ;

        int count_time=COUNT_SECONDS;//10s
        int car_count[MAX_LANE_NUM];
        int speed_count[MAX_LANE_NUM];


        int old_exist[MAX_LANE_NUM];//max lane num
        memset(old_exist,0,MAX_LANE_NUM);
        while((get_time_second()%COUNT_SECONDS)){
            this_thread::sleep_for(chrono::microseconds(1000000));
        }
        while(!quit_count){

            //if(count_time--==1){
            if(((count_time--<3)&&((get_time_second()%COUNT_SECONDS)==0))){
                string sql_time=get_sql_time();
                prt(info,"time up , liuliang  start---------------------------->");
                // lock.lock();
                int camera_size=private_data.CameraData.size();
                for(int loop_cams=0;loop_cams<camera_size;loop_cams++){
                    prt(info,"cam %d",loop_cams);
                    CameraInputData cd= private_data.CameraData[loop_cams];
                    int region_size=cd.DetectRegion.size();

                    if(region_size!=1){
                        prt(info,"flow err , region_size %d",region_size);
                        continue;
                    }

                    DetectRegionInputData id=cd.DetectRegion[0];
                    if(id.SelectedProcessor!=LABEL_PROCESSOR_MVD){
                        prt(info,"flow err , processor %s",id.SelectedProcessor);

                        continue;
                    }

                    flow_lock.lock();
                    vector< MvdProcessorOutputData>  &cam_out=outputs[loop_cams];
                    //                 prt(info,"liuliang :framesize %d",cam_out.size());
                    flow_lock.unlock();
                    //                    if(cam_out.size()<1)
                    //                        continue;
                    vector <vs_table> tables;
                    MvdProcessorInputData mvddata(id.ProcessorData);
                    prt(info,"do report start ");

                    flow_lock.lock();
                    // do_report(mvddata,cam_out,tables,sql_time);
                    do_report2(mvddata,tables,sql_time,lod[loop_cams],cd.Url);
                    flow_lock.unlock();

                    prt(info,"liuliang :table %d",tables.size());

                    for(vs_table table:tables){
                        database_insert_vs1(table);
                    }
#if 0
                    for(int loop_regions=0;loop_regions<region_size;loop_regions++){
                        prt(info,"region %d",loop_regions);
                        DetectRegionInputData dr=  cd.DetectRegion[loop_regions];
                        MvdProcessorInputData mvddata(  dr.ProcessorData);
                        int lane_size=mvddata.LaneData.size();
                        if(lane_size>=MAX_LANE_NUM){
                            prt(info,"max lane  ,err");
                            continue;
                        }

                        DatabaseInstance &ins=get_database();

                        for(int i=0;i<cam_out.size();i++){
                            prt(info,"frame %d",i);
                            MvdProcessorOutputData tmp=cam_out[i];

                            // tmp.CongestionData
                            for(int j=0;j<tmp.LaneOutputData.size();j++){
                                if(old_exist[j]==0&&tmp.LaneOutputData[j].NearCarExist)
                                {
                                    car_count[j]++;
                                    speed_count[j]+=tmp.LaneOutputData[j].VehicleSpeed;

                                }
                                old_exist[j]=tmp.LaneOutputData[j].NearCarExist;
                            }
                        }

                        int lane_count=mvddata.LaneData.size();
                        vs_table table;
                        table.record_id=int_2_string(99);
                        table.sql_time=sql_time;
                        table.period=COUNT_SECONDS;
                        table.a_id=private_data.DeviceName;
                        table.avenue="test avenue";
                        table.cameraid="test camera id";

                        for(int j=0;j<lane_count;j++){

                            //database_insert_vs(j,speed_count,car_count,sql_time);

                            MvdProcessorOutputData tmp=cam_out[j];
                            table.laneid="test laneid";
                            table.vihicle_sum=1;
                            table.sql_time=sql_time;
                            table.sql_time=sql_time;

                            //  database_insert_vs1(sql_time,lane_idaverage_speed,car_amount);
                        }

                        for(int i=0;i<cam_out.size();i++){
                            prt(info,"-->frame %d",i);
                            MvdProcessorOutputData tmp=cam_out[i];
                            // tmp.CongestionData
                            for(int j=0;j<tmp.LaneOutputData.size();j++){
                                if(old_exist[j]==0&&tmp.LaneOutputData[j].NearCarExist)
                                {
                                    car_count[j]++;
                                    speed_count[j]+=tmp.LaneOutputData[j].VehicleSpeed;

                                }
                                old_exist[j]=tmp.LaneOutputData[j].NearCarExist;
                            }
                        }



                    }//loop regions
#endif


                }//loop cams
                flow_lock.lock();
                for(int i=0;i<MAX_CAM_NUM;i++)
                    outputs[i].clear();
                flow_lock.unlock();
                count_time=COUNT_SECONDS*2;
                prt(info," stop cal ---------------------------->");
            }

            this_thread::sleep_for(chrono::microseconds(500000));
        }
    }

#define INSERT_DB_MEM(xxx)       \
    stream<< "'";\
    stream<<xxx;\
    stream<< "'";
    void database_insert_vs(int lane_index,int speed_count[],int car_count[],string sql_time)
    {
        int vs_record_id=3;

        //strstream tmp1;tmp1<<COUNT_SECONDS;
        char buf_tmp[100];memset(buf_tmp,0,100);sprintf(buf_tmp,"%d",COUNT_SECONDS);
        string vs_sp(buf_tmp);
        // string vs_sst=get_sql_time();
        string vs_sst=sql_time;
        string vs_analyce_id="2";
        string vs_savenue="12345";
        string vs_CameraID="1";
        string vs_LaneID="12";
        string vs_VSum="";//car_count
        string vs_VPSum="0";
        string vs_VNSum="0";
        string vs_VDSum="0";
        string vs_VBSum="0";
        string vs_VCSum="0";//car_count
        string vs_VMSum="0";
        string vs_VKSum="0";
        if(car_count[lane_index]){
            prt(info,"car num %d, speed total %d",car_count[lane_index],speed_count[lane_index]);
            int average_speed=speed_count[lane_index]/car_count[lane_index];
            memset(buf_tmp,0,100);
            sprintf(buf_tmp,"%d",average_speed);
            //  tmp2<<average_speed;
        }else{
            memset(buf_tmp,0,100);
            sprintf(buf_tmp,"%d",0);
        }
        string vs_ASpeed(buf_tmp);
        string vs_ATime="1";
        string vs_ASpace="1";
        string vs_AVM="1";
        string vs_AOccupy="1";
        string vs_RState="1";
        DatabaseInstance &ins=DatabaseInstance::get_instance();
        stringstream stream;
        stream<< "INSERT INTO VS \
                 ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `LaneID`,\
                   `VSum`, `VPSum`, `VNSum`, `VTSum`, `VBSum`, `VCSum`, `VMSum`,\
                   `VKSum`, `ASpeed`, `ATime`, `ASpace`, `AVM`, `AOccupy`,`RState`) VALUES";
                 stream<< "(";

        INSERT_DB_MEM(vs_record_id) stream<< ",";
        INSERT_DB_MEM(vs_sst) stream<< ",";
        INSERT_DB_MEM(vs_sp) stream<< ",";
        INSERT_DB_MEM(vs_analyce_id) stream<< ",";
        INSERT_DB_MEM(vs_savenue) stream<< ",";
        INSERT_DB_MEM(vs_CameraID) stream<< ",";
        INSERT_DB_MEM(vs_LaneID) stream<< ",";


        INSERT_DB_MEM(car_count[lane_index]) stream<< ",";
        INSERT_DB_MEM(vs_VPSum) stream<< ",";
        INSERT_DB_MEM(vs_VNSum) stream<< ",";
        INSERT_DB_MEM(vs_VDSum) stream<< ",";
        INSERT_DB_MEM(vs_VBSum) stream<< ",";
        INSERT_DB_MEM(car_count[lane_index];) stream<< ",";
        INSERT_DB_MEM(vs_VMSum) stream<< ",";

        INSERT_DB_MEM(vs_VKSum) stream<< ",";
        INSERT_DB_MEM(vs_ASpeed) stream<< ",";
        INSERT_DB_MEM(vs_ATime) stream<< ",";
        INSERT_DB_MEM(vs_ASpace) stream<< ",";
        INSERT_DB_MEM(vs_AVM) stream<< ",";
        INSERT_DB_MEM(vs_AOccupy) stream<< ",";
        INSERT_DB_MEM(vs_RState)
                stream<< ");";

        prt(info,"%s",stream.str().data());
        if(stream.str().size()>1000){
            prt(info,"sql too long");
            return;
        }
        char buf1[1000];
        memset(buf1,0,1000);
        sprintf(buf1,"%s",stream.str().data());
        ins.query(buf1);
    }

#if 1

    void database_insert_vs1(vs_table table)
    {
        string vs_record_id=table.record_id;
        string vs_sp(table.period);
        string vs_sst=table.sql_time;
        string vs_analyce_id=table.a_id;
        string vs_savenue=table.avenue;
        string vs_CameraID=table.cameraid;
        string vs_LaneID=table.laneid;
        string vs_VSum=table.vihicle_sum;
        string vs_VPSum=table.ahead_sum;
        string vs_VNSum=table.rear_sum;
        string vs_VTSum=table.truck_sum;
        string vs_VBSum=table.bus_sum;
        string vs_VCSum=table.car_sum;
        string vs_VMSum=table.motor_sum;
        string vs_VKSum=table.bicycle_sum;
        string vs_ASpeed=table.average_speed;
        string vs_ATime=table.average_time;
        string vs_ASpace=table.average_space;
        string vs_AVM=table.average_desity;
        string vs_AOccupy=table.average_occupy;
        string vs_RState=table.state;
        // DatabaseInstance &ins=DatabaseInstance::get_instance();
        DatabaseInstance &ins=get_database();
        stringstream stream;
        stream<< "INSERT INTO VS \
                 ( `RecordID`, `SST`, `SP`, `AnalyceID`, `SAvenue`, `CameraID`, `LaneID`,\
                   `VSum`, `VPSum`, `VNSum`, `VTSum`, `VBSum`, `VCSum`, `VMSum`,\
                   `VKSum`, `ASpeed`, `ATime`, `ASpace`, `AVM`, `AOccupy`,`RState`) VALUES";
                 stream<< "(";

        INSERT_DB_MEM(vs_record_id) stream<< ",";
        INSERT_DB_MEM(vs_sst) stream<< ",";
        INSERT_DB_MEM(vs_sp) stream<< ",";
        INSERT_DB_MEM(vs_analyce_id) stream<< ",";
        INSERT_DB_MEM(vs_savenue) stream<< ",";
        INSERT_DB_MEM(vs_CameraID) stream<< ",";
        INSERT_DB_MEM(vs_LaneID) stream<< ",";


        INSERT_DB_MEM(vs_VSum) stream<< ",";
        INSERT_DB_MEM(vs_VPSum) stream<< ",";
        INSERT_DB_MEM(vs_VNSum) stream<< ",";
        INSERT_DB_MEM(vs_VTSum) stream<< ",";
        INSERT_DB_MEM(vs_VBSum) stream<< ",";
        INSERT_DB_MEM(vs_VCSum) stream<< ",";
        INSERT_DB_MEM(vs_VMSum) stream<< ",";

        INSERT_DB_MEM(vs_VKSum) stream<< ",";
        INSERT_DB_MEM(vs_ASpeed) stream<< ",";
        INSERT_DB_MEM(vs_ATime) stream<< ",";
        INSERT_DB_MEM(vs_ASpace) stream<< ",";
        INSERT_DB_MEM(vs_AVM) stream<< ",";
        INSERT_DB_MEM(vs_AOccupy) stream<< ",";
        INSERT_DB_MEM(vs_RState)
                stream<< ");";
        prt(info,"%s",stream.str().data());
        if(stream.str().size()>1000){
            prt(info,"sql too long");
            return;
        }
        char buf1[1000];
        memset(buf1,0,1000);
        sprintf(buf1,"%s",stream.str().data());
        ins.query(buf1);
    }
#endif
    //////db end///////
    struct {
        int space_sum;

    };
private:
    vector <Session*> *stream_cmd;//clients who connected to our server
    string str_stream;//
    vector <Camera*> cms;
    LocationService lservice;
    ConfigManager *p_cm;
    vector <DestClient > dest_clients;
    string client_tmp_ip;
    int udp_fd;
    Timer1 watch_dog;

    bool sql_need_connect;
    thread *p_count_thread;
    vector <Mat> frames;
    bool quit_count;
    vector <Mat> buffer_frames[MAX_CAM_NUM];
    vector <Mat> buffer_frames_test;
    vector <EventRegionObjectOutput> last_events[MAX_CAM_NUM];
    vector <MvdProcessorOutputData> outputs[MAX_CAM_NUM];//support MAX_CAM_NUM cameras
    //    mutex lock;
    //    mutex buffer_lock;

    mutex flow_lock;
};


#endif // APP_H
