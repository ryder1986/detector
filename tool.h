#ifndef TOOL_H
#define TOOL_H

#include <iostream>
#include <cstdio>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstring>
#include <mutex>
#include <thread>
#include <bitset>
//#include <list>
#include <iterator>

//#include <ifaddrs.h>
//#include "tool.h"
#define NDEBUG
#include "assert.h"
using namespace std;

#ifdef IS_WIN
#include <direct.h>
#endif

#ifdef IS_UNIX
#include <sys/stat.h>
#include <execinfo.h>
#include <cxxabi.h>
#define BT_BUF_SIZE 100

#include <dirent.h>

inline int dir_count(char *dirname)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d) {
        int num=0;
        while ((dir = readdir(d)) != NULL) {
            num++;

        }
        closedir(d);
        return num;
    }
    return -1;
}
#if 0
inline void delete_dir_files(char *dirname,int total,int left)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d) {
        int num=0;
        while ((dir = readdir(d)) != NULL&&  num++<(total-left)) {

               printf("Deleteing %s,type %d  \n",dir->d_name,dir->d_type);
               char buf[1000];
               memset(buf,0,1000);
               sprintf(buf,"%s/%s",dirname,dir->d_name);
              // printf("%s\n",buf);
            if (remove(buf) == 0)
                printf("Deleted %s successfully\n",dir->d_name);
            else
                printf("Unable to  delete  %s \n",dir->d_name);
        }
        closedir(d);
    }
}
#else
// ls -tp | grep -v '/$' | tail -n +20 | xargs -d '\n' -r rm -- ; ls
// cd /ftphome/pic/ ; ls -tp  | grep -v '/$' | tail -n +20 | xargs -d '\n' -r rm --
inline void delete_dir_files(char *dirname,int total,int left)
{
   // system("cd  /ftphome/pic ; ls > /root/test.txt");
//    system("cd /ftphome/pic/ ; ls -tp  | grep -v '/$' | tail -n +1000 | xargs -d '\n' -r rm --;cd -");

     char buf[1000];memset(buf,0,1000);
     sprintf(buf,"cd %s ; ls -tp  | grep -v '/$' | tail -n +%d | xargs -d '\n' -r rm --;cd -",dirname,1000);
     system(buf);


  //    DIR *d;
//    struct dirent *dir;
//    d = opendir(dirname);
//    if (d) {
//        int num=0;
//        while ((dir = readdir(d)) != NULL&&  num++<(total-left)) {

//               printf("Deleteing %s,type %d  \n",dir->d_name,dir->d_type);
//               char buf[1000];
//               memset(buf,0,1000);
//               sprintf(buf,"%s/%s",dirname,dir->d_name);
//              // printf("%s\n",buf);
//            if (remove(buf) == 0)
//                printf("Deleted %s successfully\n",dir->d_name);
//            else
//                printf("Unable to  delete  %s \n",dir->d_name);
//        }
//        closedir(d);
//    }
}
#endif
inline string get_last_sub_string(string str,char t)
{
    unsigned int pos;
    for(pos=0;pos<str.size();pos++){
        if(str[str.size()-pos-1]==t)
            break;
    }
    string dst= str.substr(str.size()-pos,pos);
    return dst;
}
inline string get_last_sub_string(string str,char t_start,char t_end)
{
    unsigned int pos;
    for(pos=0;pos<str.size();pos++){
        if(str[str.size()-pos-1]==t_start)
            break;
    }
    unsigned int pos_end;
    for(pos_end=0;pos_end<str.size();pos_end++){
        if(str[str.size()-pos_end-1]==t_end)
            break;
    }
    string dst= str.substr(str.size()-pos,pos-pos_end-1);
    return dst;
}


//#ifdef HAVE_CXA_DEMANGLE
#if 1
inline char* demangle(const char* name)
{
    char buf[1024];
    unsigned int size=1024;
    int status;
    char* res = abi::__cxa_demangle (name,
                                     buf,
                                     ( size_t* )&size,
                                     &status);
    return res;
}
#else
const char* demangle(const char* name)
{
    return name;
}
#endif
#include <string.h>
using namespace std;
inline void print_backstrace(void)
{
    int j, nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);
    printf("@@@@@@@@@@@@@backtrace start(%d functions) \n", nptrs);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        //exit(EXIT_FAILURE);
    }
    string str;
    char   *realname;
    for (j = 0; j < nptrs; j++)
    {
        str=string(strings[j]);
        string sub=get_last_sub_string(str,'(','+');
        realname=demangle(sub.data());
        if(realname){
            string tmp(realname);
            if(tmp.size()<40)
                cout<<"--->"<<tmp<<endl;
            else
                cout<<"--->"<<tmp.substr(0,40)<<"......"<<endl;


        }
    }


    free(strings);
    cout<<"@@@@@@@@@@@@@backtrace end "<<endl;
}
#else//not unix
inline void print_backstrace(void)
{
}
#endif
using namespace std;


class LogFile{
    FILE *fp ;
public:
    LogFile(const char *fn)
    {
        log_file_open(fn);
    }
    LogFile()
    {
        log_file_open(NULL);
    }
    int write(const char *log)
    {
        fwrite(log, 1, strlen(log), fp);
        return 0;
    }

    ~LogFile()
    {
        fclose(fp);
    }
private:
    inline int log_file_open(const char *fname)
    {
        char filename[1000];
        char dirname[1000] = { "log" };
        //    FILE *tmp = fopen(dirname, "r");
        //            if(!tmp){

        //            prt(info,"quit");
        //            exit(0);
        //        }else{
        //            cout<<"exist"<<endl;
        //        }

#ifdef IS_WIN
        mkdir(dirname);
#else
        mkdir(dirname,0755);

#endif


        // FILE *fp = NULL;
        time_t timer;
        struct tm *tblock;
        /* gets time of day */
        timer = time(NULL);
        /* converts date/time to a structure */
        tblock = localtime(&timer);
        sprintf(filename, "%d_%d_%d", tblock->tm_year - 100 + 2000, tblock->tm_mon+1,
                tblock->tm_mday);
        if (fname != NULL) {
            strcpy(filename, fname);
        }
        strcat(dirname, "/");
        strcat(dirname, filename);
        fp=NULL;
        fp = fopen(dirname, "a");
        if (fp != NULL) {
            //	fwrite(log, 1, strlen(log)+1, fp);
            //   fwrite(log, 1, strlen(log), fp);
            //  fclose(fp);
        } else {
            perror("file  open fail");
            printf("file :%s\n",dirname);
        }
        return 0;
    }
};




class Tool1
{
public:
    enum LENGTH_FIXED_VALUE{
        STR_LENGTH=100,
        PATH_LENGTH=1000,
        BUFFER_LENGTH=10000
    };

    enum DEBUG_LEVEL{//choose process debug mode
        USER_DEFINED,
        DEBUG,//print most thing(show the running process)
        INFO,//routine output(necessary outputs)
        WARN,//print when warn,take care
        ERROR1,//very serious
        FATAL//most serous
    };
    enum LABEL_SELECTION{
        NORMAL=1,
        PRIVATE,
        BOTH,
        MATCHED,
        NONE
    };
public:


    static mutex lock;
private:
    static LogFile log_file1;
    //int cc=0;
    static  char filename[LENGTH_FIXED_VALUE::PATH_LENGTH];
    const int static buf_size=200;
    static int lv;//debug level,default is "DEBUG_LEVEL::INFO"
    //   static char match_label[LENGTH_FIXED_VALUE::STR_LENGTH];
    static string match_label;
    static int label_printable;//1:only print "debug to fatal" label 2:only print user-defined label 3:print both "debug to fatal" and user-defined label 4:only match match_label


    Tool1()
    {

    }
public:
    static void set_label(int label,string str=string())
    {
        match_label=str;
        label_printable=label;
    }
    static inline void set_debug_level(enum DEBUG_LEVEL level=DEBUG_LEVEL::INFO)
    {
        lv=level;
    }

    inline static string last_substr(string str,char split_ch)
    {
        char tmp[LENGTH_FIXED_VALUE::BUFFER_LENGTH];
        memset(tmp,0,LENGTH_FIXED_VALUE::BUFFER_LENGTH);
        strcpy(tmp,str.data());
        char *p=tmp;
        int total=0;
        char *p1=p;
        while(0!=*(++p)){
            if(*p==split_ch){
                p1=p+1;
                total++;
            }
        }
        return string(p1);
    }

    inline static void prt(const char *buf,const int line_no,const char *func_name,const char *file_name,const char *label,const char *time)
    {
        //        char buffer[buf_size];
        //        memset(buffer,0,buf_size);
        //        memcpy(buffer,time,strlen(time));
        //        buffer[i]='\0';

        //   cout<<"("<<buf<<")"<<'['<<line_no<<']'<<'['<<func_name<<']'<<'['<<file_name<<']'<<'['<<buffer<<']'<<'['<<label<<']'<<endl;
        //  cout<<"("<<buf<<")"<<'['<<line_no<<']'<<'['<<func_name<<']'<<'['<<file_name<<']'<<'['<<buffer<<']'<<'['<<label<<']'<<endl;

        string fn(file_name);
#if 0
#ifdef  IS_UNIX
        char t='/';
#elseik
        char t='\\';
#endif
        cout<<last_substr(fn,t).data()<<"(line"<<line_no<<")"<<":"<<buf<<endl;
#endif
        //     cout<<last_substr(last_substr(fn,'/'),'\\').data()<<"(line"<<line_no<<")"<<":"<<buf<<endl;
        int debug_level=0;
        if(!strcmp("info",label)){
            debug_level=DEBUG_LEVEL::INFO;
        }else         if(!strcmp("debug",label)){
            debug_level=DEBUG_LEVEL::DEBUG;
        }else         if(!strcmp("warn",label)){
            debug_level=DEBUG_LEVEL::WARN;
        }else         if(!strcmp("error",label)){
            debug_level=DEBUG_LEVEL::ERROR1;
        }else         if(!strcmp("fatal",label)){
            debug_level=DEBUG_LEVEL::FATAL;
        }else {
            debug_level=DEBUG_LEVEL::USER_DEFINED;
        }
        //        cout<<debug_level<<endl;
        //        cout<<label_printable<<endl;
        //         cout<<debug_level<<endl;
        //          cout<<lv<<endl;
        bool flg=false;
        //        if(DEBUG_LEVEL::INFO<=debug_level&&debug_level<=DEBUG_LEVEL::FATAL)
        //            flg=true;

        switch(label_printable){
        case LABEL_SELECTION::BOTH:
            if((lv<=debug_level&&debug_level<=DEBUG_LEVEL::FATAL)||debug_level==DEBUG_LEVEL::USER_DEFINED)
                flg=true;
            break;
        case LABEL_SELECTION::NORMAL:
            if(lv<=debug_level&&debug_level<=DEBUG_LEVEL::FATAL)
                flg=true;
            break;
        case LABEL_SELECTION::MATCHED:
            if(debug_level==DEBUG_LEVEL::USER_DEFINED&&!strcmp(label,match_label.data()))
                flg=true;
            break;
        case LABEL_SELECTION::PRIVATE:
            if(debug_level==DEBUG_LEVEL::USER_DEFINED)
                flg=true;
            break;
        case LABEL_SELECTION::NONE:break;
        }
        string tmp_str;
        tmp_str.clear();
        char buf1[100];
        memset(buf1,0,100);
        sprintf(buf1,"%d",line_no);
//#define WITHOUT_TIME_LABEL
#ifdef WITHOUT_TIME_LABEL
        time="";
        label="";
#endif
        if(flg){
            tmp_str.append("[").append(time).append("][").append(label).append("][").\
                    append(last_substr(last_substr(fn,'/'),'\\').data()).append(",").append(buf1).append(",").append(func_name).append("]===>").append(buf).append("\n");
            cout<<tmp_str;
            log_file1.write(tmp_str.data());
            fflush(stdout);
            //  fflush(NULL);
        }
    }
    inline static char* get_time_n()
    {
        chrono::system_clock::time_point today= chrono::system_clock::now();
        time_t tt= chrono::system_clock::to_time_t(today);
        return ctime(&tt);
    }
    inline static string get_time()
    {
        chrono::system_clock::time_point today= chrono::system_clock::now();
        time_t tt= chrono::system_clock::to_time_t(today);
        string tm(ctime(&tt));
        unsigned int pos=tm.find('\n');
        if(pos!=string::npos){
            tm[pos]=0;
        }

        return tm;
    }

    static int  aaaa;
    static const int ss=123;
    enum test{
        ABC=1,
        BCD=2
    };
    typedef test sss;

    void aaa()
    {



    }




};

#include <functional>
/*
    This is a versy timer
*/
class Timer1{
    typedef function<void()> timed_func_t;
public:
    Timer1(timed_func_t user_func):quit(false),func(user_func),interval(1000000)
    {
        quit_done=false;
    }

    void start(int inter)
    {
        interval=inter;
        thread([this]{
            while(!quit){
                func();
                this_thread::sleep_for(chrono::milliseconds(interval));
            }
            quit_done=true;
        }).detach();
    }
    void stop()
    {
        quit=true;
        while(!quit_done){
            this_thread::sleep_for(chrono::microseconds(1));
        }
    }

private:
    timed_func_t func=[]{cout<<"warn: undefined function"<<endl;};
    bool quit;
    int interval;
    bool quit_done;
};
/*
    This is a   timer which take 1 arg,TODO:return ?
*/
class Timer2{
public:
    template<typename callable, class... arguments>
    void SyncWait(int after, callable&& f, arguments&&... args){

        std::function<typename std::result_of<callable(arguments...)>::type()> task
                (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        std::this_thread::sleep_for(std::chrono::milliseconds(after));
        task();
    }
    template<typename callable, class... arguments>
    void AsyncWait(int after, callable&& f, arguments&&... args){
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        std::thread([after, task](){
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
    }
public:
    Timer2()
    {

    }

private:
};

class JsonStr{
public:

    static inline bool get_valid_buf(string &src,string &dst)
    {
        if(try_get_obj_buf(src,dst)){
            //src.remove(0,dst.size());
            src=src.substr(dst.size(),src.size());
            return true;
        }
        return false;
    }
    static inline string remove_prefix(string src)
    {
        int n=src.find_first_of("{");
        return src=src.substr(n,src.size()-n);
    }
private:
    static inline bool try_get_obj_buf(string src,string &dst)
    {
        int ret=false;
        int stack=0;
        const char *p_tmp=src.data();
        bool flg=false;
        dst.clear();
        dst.append(src);
        int i;
        if(count_begin_symbol(src)>0){
            for(i=0;i<src.size();i++){
                if(p_tmp[i]=='{')
                {
                    stack++;
                    flg=true;
                }
                if(p_tmp[i]=='}')
                    stack--;
                if(stack==0&&flg)
                {
                    break;
                }

            }
            if(i<src.size()){
                ret=true;
                if(src[i+1]=='\n')
                    //   dst.truncate(i+2);
                    dst=dst.substr(0,i+2);
                else
                    // dst.truncate(i+i);
                    dst=dst.substr(0,i+1);
            }
        }
        return ret;
    }
    static inline int count_begin_symbol(string ba)
    {
        const char *tmp=ba.data();
        int sz=ba.size();
        int ret=0;
        int i;
        for( i=0;i<sz;i++){
            if(tmp[i]=='{'){
                ret++;
            }
        }
        return ret;
    }
};
#define prt(label,...) {Tool1::lock.lock(); char buf[Tool1::LENGTH_FIXED_VALUE::BUFFER_LENGTH];memset(buf,0,sizeof(buf));snprintf(buf,sizeof(buf),__VA_ARGS__);\
    Tool1::prt(buf,__LINE__,__FUNCTION__,__FILE__,#label,Tool1::get_time().data());Tool1::lock.unlock();}
#define THREAD_DEF(cls,fun) new thread(std::mem_fn(&cls::fun),*(cls*)this);
template<typename callable,class...arguments>
void _start(callable&& f, arguments&&... args)
{
    //prt(info,"test start>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    //prt(info,"%s",  typeid(f).name());
    std::function<typename std::result_of<callable(arguments...)>::type()> task
            (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    task();

    //prt(info,"test end<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
template<typename callable,class...arguments>
void _start_async(callable&& f, arguments&&... args)
{
    //prt(info,"test start>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    //prt(info,"%s",  typeid(f).name());
    std::function<typename std::result_of<callable(arguments...)>::type()> task
            (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    thread(task).detach();

    //prt(info,"test end<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
template <typename T>
void del_from_vector(int del_index,T &con )
{
    if(del_index>0&&del_index<=con.size()){
        delete con[del_index-1];
        con.erase(con.begin()+del_index-1);
    }else{
        prt(info,"invalid del index %d",del_index);
    }
}
template <typename T,typename F>
void insert_into_vector(int insert_index,T &con ,F mem )
{
    if(insert_index>=0&&insert_index<=con.size()){

        con.insert(con.begin()+insert_index,mem);
    }else{
        prt(info,"invalid del index %d",insert_index);
    }
}
#ifdef IS_UNIX
#include <sys/time.h>
inline long get_time()
{
    time_t tt;
    struct timeval tv;
    tt=time(NULL);
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}
inline long get_time_second()
{
    time_t tt;
    struct timeval tv;
    tt=time(NULL);
    gettimeofday(&tv,NULL);
    return  tv.tv_sec;
    //return tv.tv_sec*1000+tv.tv_usec/1000;
}
inline long get_time_point_ms()
{
    return get_time();
}


inline  char *get_sql_time()
{
    static char buf2[100];// = "2000-01-01-00:00:00";
    memset(buf2,0,100);
    char fmt[] = "%Y-%m-%d-%H:%M:%S";
    time_t tt;
    tt=time(NULL);
    tm*  lt=localtime(&tt);
    if (strftime(buf2,100, fmt, lt) != NULL) {
        //prt(info,"ok");
    }else{
        prt(info,"err");
    }
    return buf2;
}

inline char * get_time_string()
{
    time_t tt;
    struct timeval tv;
    tt=time(NULL);
    tm*lt=   localtime(&tt);
    cout<<  ctime(&tt);
    return ctime(&tt);
//    gettimeofday(&tv,NULL);
//    return tv.tv_sec*1000+tv.tv_usec/1000;
}
#endif

#define PRT_DECODE_EXCEPTION prt(debug,"error in decoding %s data:=>%s<=",typeid(this).name(),config.str().data());
//#define PRT_DECODE_EXCEPTION prt(info,"error in decoding %s data:=>%s<=",typeid(this).name(),config.str().data());
//#define PRT_ENCODE_EXCEPTION prt(info,"error in encoding %s data:=>%s<=",typeid(this).name(),config.str().data());
#define PRT_ENCODE_EXCEPTION prt(debug,"error in encoding %s data:=>%s<=",typeid(this).name(),config.str().data());


#define PRT_DECODE_EXCEPTION prt(debug,"error in decoding %s data:=>%s<=",typeid(this).name(),config.str().data());
#define PRT_ENCODE_EXCEPTION prt(debug,"error in encoding %s data:=>%s<=",typeid(this).name(),config.str().data());
//#define PRT_DECODE_EXCEPTION prt(info,"error in decoding %s data:=>%s<=",typeid(this).name(),config.str().data());
//#define PRT_ENCODE_EXCEPTION prt(info,"error in encoding %s data:=>%s<=",typeid(this).name(),config.str().data());
class ClientConfig{
public:
    static bool show_processor_text;
    static bool show_camera_state;
};
#define SLEEP_HERE     \
while(true){\
this_thread::sleep_for(chrono::microseconds(1000));\
}
#define PAUSE_HERE_FOREVER     \
while(true){\
this_thread::sleep_for(chrono::microseconds(1000));\
}
#define SLEEP_HERE_MS(ms)     \
do{\
this_thread::sleep_for(chrono::milliseconds(ms));\
}while(false)
#define PAUSE_HERE_3s     \
while(true){\
static int tmp=0;if(tmp++>3){prt(info,"pause 3 seconds done");break;}this_thread::sleep_for(chrono::microseconds(1000000));\
}

#define DELETE_POINTER_LATER(tp,name,msec)\
{\
tp addr=name;\
    \
std::thread([addr](){\
    this_thread::sleep_for(chrono::milliseconds(msec));\
    delete addr;/*prt(info,"delete %p ok",addr);*/\
}\
).detach();\
    \
name=NULL;\
}
inline string int_2_string(int num)
{
    char buf[1000];
    memset(buf,0,1000);
    sprintf(buf,"%d",num);
    string str(buf);
    return str;
}

#endif // TOOL1_H
