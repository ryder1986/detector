#ifndef JSONPACKET_H
#define JSONPACKET_H

#include <cstring>
#include <json/json.h>
#include <json/value.h>
#include <tool.h>
using namespace std;
using  namespace Json ;

class JsonPacket:public Value
{
public:
    typedef Value JsonValue;
    JsonPacket(string str)
    {
        str_data=str;
        JsonValue v;
        Reader r;
        bool rst=r.parse(str,v);
        if(!rst){
            prt(info,"parse err");
        }else
            val=v;
    }

    JsonPacket(vector<JsonPacket> ar)
    {
        JsonValue v;
        int sz=ar.size();
        for(int i=0;i<sz;i++){
            v[i]=ar[i].val;
        }
        val=v;
    }

    JsonPacket()
    {
    }

    void operator =(string str)
    {
        JsonValue v;
        Reader r;
        bool rst=r.parse(str,v);
        if(!rst){
            prt(info,"parse err");
        }else
            val=v;
    }

    void operator =(JsonPacket pkt)
    {
        val=pkt.value();
        str_data=pkt.str();
    }

    void operator =(vector<JsonPacket> ar)
    {
        JsonValue v;
        int sz=ar.size();
        for(int i=0;i<sz;i++){
            v[i]=ar[i].val;
        }
        val=v;
    }
    template <typename T>
    void set(string name,T value)
    {
        if(!name_exist(name,"set")){
            prt(info,"no keyword:%s",name.data());
            // throw Exception("no keyword");
            //print_backstrace();
        }
        else
            val[name]=value;
    }
    void set(string name,JsonPacket p)
    {
        if(!name_exist(name,"set")){
            prt(info,"setting name: %s fail",name.data());
            //print_backstrace();
        }
        else
            val[name]=p.val;
    }

    void set(string name,vector<JsonPacket> pkts)
    {
        val[name].clear();
        if(!name_exist(name,"set")){
            prt(info,"setting name: %s fail",name.data());
        }
        else
        {
            for(JsonPacket p:pkts)
                val[name].append(p.value());
        }
    }
    JsonPacket get(string name)
    {
        if(name_exist(name,"get")){
            JsonPacket pkt(val[name]);
            return pkt;
        }
        else{
            return JsonPacket();
        }
    }
    string get_string(string name)
    {
        if(name_exist(name,"get")){
            return    val[name].asString();

        }
        else
            return string();
    }
    int get_int(string name)
    {
        if(name_exist(name,"get")){
            return    val[name].asInt();

        }
        else
            return 0;
    }
    bool get_bool(string name)
    {
        if(name_exist(name,"get")){
            return  val[name].asBool();
        }
        else
            return false;
    }
    float get_float(string name)
    {
        if(name_exist(name,"get")){
            return  val[name].asFloat();

        }
        else
            return false;
    }
    vector <JsonPacket> get_array(string name)
    {
        vector <JsonPacket>  pa;
        if(name_exist(name,"get")){
            JsonValue v=val[name];
            bool is_a= v.isArray();
            if(is_a){
                int sz=v.size();
                for(int i=0;i<sz;i++){
                    pa.push_back(v);
                }
            }
            return pa;
        }
        else
            return pa;
    }
public:
    void clear()
    {
        val.clear();
    }

    string str()
    {
#if 1
        //FastWriter  w;
        FastWriter  w;
        //StyledWriter  w;
        return  w.write(val);
#else
        return val.asString();
#endif
    }
    JsonValue &value()
    {
        return val;
    }
    bool is_null()
    {
        return val.isNull();
    }

    void add(string name,vector<JsonPacket> pkts)
    {
        val[name].clear();
        for(JsonPacket p:pkts)
            val[name].append(p.value());
    }
    void add(string name,JsonPacket pkt)
    {
        val[name].clear();
        val[name]=pkt.value();
    }
    template <typename T>
    void add(string name,T value)
    {
        val[name]=value;
    }
    template <typename T>
    void add(string name,vector<T> va)
    {
        for(T t:va)
            val[name].append(t);
    }

    int to_int()
    {
        if(val.empty()){
            //assert(false);
            //int t=1/0;//sig fault deal to error.

            //  prt(info,"to int error :no content,%d",t);
        //     Exception e("json to int fail");
            throw exception();
            print_backstrace();
            return 0;
        }
        if(!val.isInt()){
            prt(info,"get error type:not Int");
            return 0;
        }
        return val.asInt();
    }
    vector <int> to_int_array()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to int error :no content");
            print_backstrace();
            return  vector <int>();
        }
        int sz=val.size();
        vector <int> rt;
        for(int i=0;i<sz;i++){
            rt.push_back(val[i].asInt());
        }
        return rt;
    }
    double to_double()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to  double :no content");
            return 0;
        }
        if(!val.isDouble()){
            prt(info,"get error type:not Double");
            return 0;
        }
        return val.asDouble();
    }
    vector <double>to_double_array()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to  double :no content");
            return  vector <double>();
        }
        int sz=val.size();
        vector <double> rt;
        for(int i=0;i<sz;i++){
            rt.push_back(val[i].asDouble());
        }
        return rt;
    }
    bool to_bool()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to  bool :no content");
            return false;
        }
        if(!val.isBool()){
            prt(info,"get error type:not Bool");
            return false;
        }
        return val.asBool();
    }
    vector<bool>to_bool_array()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to  bool :no content");
            return  vector<  bool >();
        }
        vector <bool> rt;
        int  sz=val.size();
        for(int i=0;i<sz;i++){
            rt.push_back(val[i].asBool());
        }
        return rt;
    }
    string to_string()
    {
        if(val.empty()){
            throw exception();
            assert(!val.empty());
            //throw Exception();
        }
        if(val.empty()){
            throw exception();
            prt(info,"to  string :no content, return a null string");
            return string();
        }
        if(!val.isString()){
            prt(info,"get error type:not String");
            return string();
        }
        return val.asString();
    }
    vector<string> to_string_array()
    {
        if(val.empty()){
            throw exception();
            prt(info,"to  string :no content");
            return  vector<  string> ();
        }
        int sz=val.size();
        vector <string> rt;
        for(int i=0;i<sz;i++){
            rt.push_back(val[i].asString());
        }
        return rt;
    }
    vector<JsonPacket> to_array()
    {
        vector<JsonPacket>  ar;
        if(val.empty()){
            throw exception();
            assert(!val.empty());
            //throw Exception("empty val");
        }
        if(val.empty()){
            return ar;
        }
        if(!val.isArray()){
            prt(info,"get error type:not array");
            show_value_type(val);
            return ar;
        }
        for(Value v:val){
            ar.push_back(v);
        }
        return ar;
    }

private:
    JsonPacket(JsonValue v)
    {

        val=v;
        str_data=str();
    }
    void show_value_type(JsonValue val)
    {
        prt(info,"show value");
        if(val.isArray()){  prt(info,"type:array");}
        if(val.isString()){  prt(info,"type:str");}
        if(val.empty()){  prt(info,"type:empty");}
        if(val.isBool()){  prt(info,"type:bool");}
        if(val.isInt()){  prt(info,"type:int");}
        if(val.isObject()){  prt(info,"type:obj");}
        prt(info,"show value done");

    }
    bool name_exist(string name,string str)
    {
        bool rst=true;
        JsonValue v=val;
        rst=v[name].isNull();
        if(rst&&(str=="get"||str=="set")){
            //throw exception();
            //prt(info," (%s) not exist or no data",name.data());
            //print_backstrace();
            return false;
        }
        return true;
    }
    void check_type(JsonValue v)
    {
        if(v.isArray())   prt(info,"isArray");
        if(v.isBool())   prt(info,"isBool");
        if(v.isDouble())   prt(info,"isDouble");
        if(v.isInt())   prt(info,"isInt");
        if(v.isInt64())   prt(info,"isInt64");
        if(v.isIntegral())   prt(info,"isIntegral");
        if(v.isNull())   prt(info,"isNull");
        if(v.isNumeric())   prt(info,"isNumeric");
        if(v.isObject())   prt(info,"isObject");
        if(v.isString())   prt(info,"isString");
        if(v.isUInt())   prt(info,"isUInt");
        if(v.isUInt64())   prt(info,"isUInt64");
    }
private:
    JsonValue val;
    string str_data;
};

class JsonData{
protected:
    JsonPacket config;
public:
    JsonData(JsonPacket pkt)
    {
        config=pkt;
    }
    JsonData()
    {
    }
    virtual void encode()=0;
    virtual void decode()=0;
    static JsonPacket get_request_pkt(int op, int index, JsonPacket data);
    JsonPacket data()
    {
        return config;
    }
};
class JsonDataWithTitle:public JsonData{
private:
    string title;
public:
    JsonDataWithTitle(JsonPacket pkt1,string tt):JsonData(pkt1.get(tt))
    {
        title=tt;
    }
    JsonDataWithTitle()
    {
    }
    JsonDataWithTitle(string tt):title(tt)
    {

    }
//    JsonPacket data()
//    {
//        JsonPacket pkt;
//        pkt.set(title,config);
//    }
};
template<typename TP>
class VdData{
protected:
    TP private_data;
public:
    VdData()
    {

    }
    VdData(TP d):private_data(d)
    {
    }
    virtual ~VdData()
    {
    }
    TP get_data()
    {
        return private_data;
    }
};

#define DECODE_INT_MEM(mem) {this->mem=config.get(#mem).to_int();}
#define DECODE_STRING_MEM(mem) {this->mem=config.get(#mem).to_string();}
#define DECODE_DOUBLE_MEM(mem) {this->mem=config.get(#mem).to_double();}
#define DECODE_BOOL_MEM(mem) {this->mem=config.get(#mem).to_bool();}

#define DECODE_INT_ARRAY_MEM(mem) {this->mem=config.get(#mem).to_int_array();}
#define DECODE_STRING_ARRAY_MEM(mem) {this->mem=config.get(#mem).to_string_array();}
#define DECODE_DOUBLE_ARRAY_MEM(mem) {this->mem=config.get(#mem).to_double_array();}
#define DECODE_BOOL_ARRAY_MEM(mem) {this->mem=config.get(#mem).to_bool_array();}


#define DECODE_PKT_MEM(mem) {this->mem=config.get(#mem);}
#define DECODE_PKT_ARRAY_MEM(mem)  {auto tmp=config.get(#mem).to_array();this->mem.assign(tmp.begin(),tmp.end());}
#define DECODE_JSONDATA_MEM(mem) {this->mem=config.get(#mem);}
#define DECODE_JSONDATA_ARRAY_MEM(mem)  {auto tmp=config.get(#mem).to_array();this->mem.assign(tmp.begin(),tmp.end());}

#define ENCODE_MEM(mem) {config.add(#mem,this->mem);}

#define ENCODE_INT_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_STRING_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_DOUBLE_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_BOOL_MEM(mem) {ENCODE_MEM(mem);}


#define ENCODE_INT_ARRAY_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_STRING_ARRAY_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_DOUBLE_ARRAY_MEM(mem) {ENCODE_MEM(mem);}
#define ENCODE_BOOL_ARRAY_MEM(mem) {ENCODE_MEM(mem);}


#define ENCODE_PKT_MEM(mem) {config.add(#mem,this->mem);}
#define ENCODE_PKT_ARRAY_MEM(mem) {config.add(#mem,mem);}
#define ENCODE_JSONDATA_MEM(mem) {config.add(#mem,this->mem.data());}
#define ENCODE_JSONDATA_ARRAY_MEM(mem) { vector<JsonPacket> pkts;\
    for(auto tmp1:this->mem){pkts.push_back(tmp1.data());}\
    config.add(#mem,pkts);}


class RequestPkt:public JsonData
{
public:
    int Operation;
    int Index;
    JsonPacket Argument;

    RequestPkt(int op,int index , JsonPacket data,JsonPacket rt=JsonPacket()):Operation(op),Index(index),Argument(data)
    {
        encode();
    }
    RequestPkt(JsonPacket p):JsonData(p)
    {
        decode();
    }
    RequestPkt()
    {

    }

    void encode()
    {
        ENCODE_INT_MEM(Index);
        ENCODE_INT_MEM(Operation);
        ENCODE_PKT_MEM(Argument);
    }
    void decode()
    {
        DECODE_INT_MEM(Operation);
        DECODE_INT_MEM(Index);
        DECODE_PKT_MEM(Argument);
    }

};
class ReplyPkt:public JsonData
{
public:
    bool Ret;
    int Operation;
    JsonPacket Data;
    ReplyPkt(bool p,int o,JsonPacket pkt):Ret(p),Operation(o),Data(pkt)
    {
        encode();
    }
    ReplyPkt()
    {

    }
    ReplyPkt(string ret):JsonData(ret)
    {
        decode();
    }
    void encode()
    {
        ENCODE_BOOL_MEM(Ret);
        ENCODE_INT_MEM(Operation);
        ENCODE_PKT_MEM(Data);
    }
    void decode()
    {
        DECODE_BOOL_MEM(Ret);
        DECODE_INT_MEM(Operation);
        DECODE_PKT_MEM(Data);
    }

};

class VdRect:public JsonData
{
public:
    int x;
    int y;
    int w;
    int h;
    VdRect(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    VdRect(int x1,int y1,int w1,int h1):x(x1),y(y1),w(w1),h(h1)
    {
        encode();
    }
    VdRect(){}
    void decode()
    {
        DECODE_INT_MEM(x);
        DECODE_INT_MEM(y);
        DECODE_INT_MEM(w);
        DECODE_INT_MEM(h);

    }
    void encode()
    {
        ENCODE_INT_MEM(x);
        ENCODE_INT_MEM(y);
        ENCODE_INT_MEM(w);
        ENCODE_INT_MEM(h);
    }

};
class ObjectRect:public JsonData
{
public:
    int x;
    int y;
    int w;
    int h;
    string label;
    int confidence_rate;
    ObjectRect(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    ObjectRect(int x1,int y1,int w1,int h1,string l,int c):x(x1),y(y1),w(w1),h(h1),label(l),confidence_rate(c)
    {
        encode();
    }
    ObjectRect(){}
    void decode()
    {
        DECODE_INT_MEM(x);
        DECODE_INT_MEM(y);
        DECODE_INT_MEM(w);
        DECODE_INT_MEM(h);
        DECODE_STRING_MEM(label);
        DECODE_INT_MEM(confidence_rate);
    }
    void encode()
    {
        ENCODE_INT_MEM(x);
        ENCODE_INT_MEM(y);
        ENCODE_INT_MEM(w);
        ENCODE_INT_MEM(h);
        ENCODE_STRING_MEM(label);
        ENCODE_INT_MEM(confidence_rate);
    }

};
class VdPoint:public JsonData
{
public:
    int x;
    int y;
    VdPoint(JsonPacket pkt):JsonData(pkt)
    {
        decode();
    }
    VdPoint(int x,int y):x(x),y(y)
    {
        encode();
    }
    VdPoint()
    {

    }
//    void    operator =(VdPoint p)
//    {
//        x=p.x;
//        y=p.y;
//        encode();
//    }
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

template <typename T>
vector<JsonPacket> obj_2_pkt_array(T jd)
{
    vector<JsonPacket> v;
    for(JsonData d:jd){
        v.push_back(d.data());
    }
    return v;
}
template <typename T>
JsonPacket obj_2_pkt(T jd)
{

    return jd.data();
}

class PaintableData
{
public:
    enum Colour{
        Red=1,
        Green,
        Blue
    };
    enum Event{
        MoveVer=1,
        MoveAll
    };
    PaintableData() {
        seizing=false;
        point_index=0;
        event_type=0;
        ori_pnt=VdPoint(0,0);
    }
//    PaintableData(PaintableData &pd) {
//        seizing=pd.seizing;
//        point_index=pd.point_index;
//        event_type=pd.event_type;
//        ori_pnt=pd.ori_pnt;
//    }
public:
    inline int p_on_v(const vector <VdPoint> points,VdPoint p,int distance=10)
    {
        for(int i=0;i<points.size();i++){
            //prt(info,"%d -> %d",points[i].x,p.x);
            if((abs(points[i].x-p.x)<distance)&&((abs(points[i].y-p.y))<distance)){
                return (i+1);
            }
        }
        return 0;
    }

    inline bool p_on_l(VdPoint b,VdPoint e, VdPoint dst,int distance=20)
    {
        bool v1= (((dst.x<b.x+distance)||(dst.x<e.x+distance))&&((dst.x>b.x-distance)||(dst.x>e.x-distance)));
        bool v2=(((dst.y<b.y+distance)||(dst.y<e.y+distance))&&((dst.y>b.y-distance)||(dst.y>e.y-distance)));
        bool v3= (abs(((dst.x-e.x)*(dst.y-b.y))-((dst.y-e.y)*(dst.x-b.x)))<1000);
        //  bool v3= true;
        if(v1&&v2&&v3)
            return true;
        else
            return false;
    }
    inline bool p_on_ls(const vector <VdPoint> points,VdPoint p,int distance=10)
    {
        for(int i=0;i<points.size()-1;i++){
            if(p_on_l(points[i],points[i+1],p)){
                return true;
            }
        }
        if(p_on_l(points[0],points[points.size()-1],p)){
            return true;
        }
        return false;
    }
    inline bool p_on_vl(const vector <VdPoint> points,  VdPoint dst)
    {
        if(points.size()>1){
            for(int i=1;i<points.size();i++){
                if(p_on_l(points[i-1],points[i],dst)){
                    return true;
                }
            }
            if(p_on_l(points.front(),points.back(),dst)){
                return true;
            }
        }
        return false;
    }
#if 0
    virtual bool press(VdPoint pnt)=0;
    virtual bool move(VdPoint pnt)=0;
    virtual bool double_click(VdPoint pnt)=0;
    virtual void release()=0;
#else
    virtual bool press(VdPoint pnt){return false;}
    virtual bool move(VdPoint pnt){return false;}
    virtual bool double_click(VdPoint pnt){return false;}
    virtual void release(){}
#endif
    template <typename A,typename B,typename C>
    void draw(A draw_line,B draw_circle,C draw_text);
    void release_event()
    {
        seizing=false;
        point_index=0;
        event_type=0;
    }
public:
    bool seizing;
    int event_type;
    VdPoint ori_pnt;
    int point_index;
};

static inline VdRect vers_2_rect(vector <VdPoint> area)
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
static inline VdPoint add_point_offset(VdPoint p_ori,VdPoint p_offset)
{
     return VdPoint(p_ori.x+p_offset.x,p_ori.y+p_offset.y);
}
#endif // JSONPACKET_H
