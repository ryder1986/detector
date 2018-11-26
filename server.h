#ifndef SERVER_H
#define SERVER_H
#include "tool.h"
#ifdef IS_UNIX
#include "socket.h"
#endif
#define BUF_SIZE 10000
//#define BUF_SIZE 10//if size if 10,
//recived byte sequence maybe interupt by other connection,and become disorder
#define IP_STR_LEN 20
class Session{
public:
    Session(int fd,string ip,int port):quit(false),client_ip(ip),
        client_port(port)
    {
        skt=fd;
        int ul = 0;
        ioctl(fd, FIONBIO, &ul);
        prt(info,"handle new connection: %s ",ip.data());
       // auto func_recv=bind(&Session::recv,this);
        auto func_recv=bind(&Session::recv_json,this);


        trd=new thread([func_recv](){func_recv();});
    }
    ~Session()
    {
        prt(info,"quit connection: %s ",ip().data());
        quit=true;
        trd->join();
        delete trd;

    }

    int send(const char *buf,int len)
    {
        int ret= Socket::SendDataByTcp(skt,buf,len);

        if(ret){
            prt(info,"send start(%d bytes)<========",ret);
            printf("%s",buf);
            prt(info,"send end(%d bytes)<========",ret);
        }else{
            prt(info,"send fail",ret);
        }
        return ret;
    }
    int recv()
    {
        while(!quit){
            memset(buf,0,BUF_SIZE);
            int ret=Socket::RecvDataByTcp1(skt,buf,BUF_SIZE);
            if(ret){
                prt(info,"read %d bytes",ret);
                process_data(this,buf,ret);
            }else{
                prt(info,"read socket error");
                break;
                cout<<"socket maybe closed,retry read after 1sec"  <<endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        end_this(this);
        return 0;
    }


    string str_stream;
    int recv_json()
    {
        while(!quit){
            memset(buf,0,BUF_SIZE);
            int ret=Socket::RecvDataByTcp1(skt,buf,BUF_SIZE);
            if(ret){
                prt(info,"read %d bytes",ret);



                string valid_buf;
                valid_buf.clear();
                str_stream.append(string(buf,ret));
                if(JsonStr::get_valid_buf(str_stream,valid_buf)) {//Get valid json object, TODO:we only check {} matches, we should check json grammar
                    prt(info,"recive object--> %s(%d bytes left)",valid_buf.data(),str_stream.size());
                    try{
                        string json_buf=JsonStr::remove_prefix(valid_buf);
//
                        process_data(this,(char*)json_buf.data(),json_buf.size());

                       // RequestPkt event(json_buf);
//                        ReplyPkt ret_pkt;
//                        client_tmp_ip=clt->ip();
//                        process_ret=process_event(event,ret_pkt);
//                        ret_str=ret_pkt.data().str();
//                        clt->send(ret_str.data(),ret_str.length());
                    }catch(exception e){
                        prt(info,"recive error json ");
                    }
                }
            }else{
                prt(info,"read socket error");
                break;
                cout<<"socket maybe closed,retry read after 1sec"  <<endl;
                this_thread::sleep_for(chrono::seconds(1));
            }




        }
        end_this(this);
        return 0;
    }
    string ip()
    {
        return client_ip;
    }
    int port()
    {
        return client_port;
    }
private:
    int skt;
    bool quit;
    char buf[BUF_SIZE];
    std::thread *trd;
    string client_ip;
    int client_port;
public:
    function <void(Session *)> end_this=[](Session *){ cout<<"we  can quit now"<<endl;};
    function <void(Session *,char *data,int len)> process_data=[](Session *,char *data,int len){ cout<<"process data"<<endl;};

};


class Tcpserver{
    function <void(Session *,char *,int)>fct;
public:
    Tcpserver(int p):port(p),quit(false)
    {
        fd= Socket::StartTcpServerSock(port,1000,1000);
        if(fd>0){
            auto func=bind(&Tcpserver::listen,this);
            trd=new thread([func](){func();});
        }else{
            prt(info,"bind port fail");
        }
    }
    Tcpserver(int p,function <void(Session *,char *,int)>fc):port(p),quit(false),fct(fc)
    {
        fd= Socket::StartTcpServerSock(port,1000,1000);
        if(fd>0){
            auto func=bind(&Tcpserver::listen,this);
            trd=new thread([func](){func();});
        }else{
            prt(info,"bind port fail");
        }
    }
    Tcpserver(vector<Session*> *&clts,int p,function <void(Session *,char *,int)>fc):port(p),quit(false),fct(fc)
    {
        clts=&clients;
        fd= Socket::StartTcpServerSock(port,1000,1000);
        if(fd>0){
            auto func=bind(&Tcpserver::listen,this);
            trd=new thread([func](){func();});
        }else{
            prt(info,"bind port fail");
        }
    }
    ~Tcpserver()
    {
        quit=true;
        if(trd){
            trd->join();
            int sz=clients.size();
            for(int i=sz-1;i>=0;i--)
            {
                delete clients[i];
                clients.pop_back();
            }
            delete trd;
        }
    }

    void listen()
    {
        while(!quit){

            memset(peer_ip,0,IP_STR_LEN);
            int cl_fd=Socket::WaitTcpConnect(fd,1,peer_ip,&peer_port);
            if(cl_fd>0){
                prt(info,"get connect from %s:%d",peer_ip,peer_port);
                Session *s=new Session(cl_fd,peer_ip,peer_port);
                s->end_this=bind(&Tcpserver::quit_session,this,placeholders::_1);
                s->process_data=fct;
                clients.push_back(s);
            }
        }
    }
    void send_test()
    {
        if(clients.size())
        {
            clients[0]->send("123456",6);
        }
    }
    void size()
    {
        cout<<clients.size();
    }
private:
    void quit_session(Session *s)
    {
        vector <Session *>::iterator e=find(clients.begin(),clients.end(),s);
        clients.erase(e);
        thread([s](){
            delete s;
        }).detach();
        s=NULL;
    }

    bool quit;
    int port;
    std::thread *trd;
    unsigned short peer_port;
    int fd;
    char peer_ip[IP_STR_LEN];
public:
    vector <Session *> clients;

};
// check port , once "pedestrian" recived , send IP to peer
class LocationService{
    int fd;
public:
    LocationService()
    {
        fd=Socket::CreatBroadcast(12348);
        prt(info,"start location");
        thread([this](){
            while(1){
                this_thread::sleep_for(chrono::microseconds(100));
                string peer_ip;
                int peer_port;
                if(check_port(peer_ip,peer_port)){
                    string ip=get_ip();
                    udp_send(peer_ip,peer_port,(char *)ip.data(),ip.length());
                }
            }

        }).detach();
    }
private:
    // client will send  pedestrian to  12348
    bool check_port(string &peer_ip,int &peer_port)
    {
        //   int fd=Socket::CreatBroadcast(12348);
        unsigned char buf[100];
        memset(buf,0,100);
        int rev_len=Socket::RecvFromBroadcast(buf,strlen("pedestrian"),fd,peer_ip,peer_port);
        // close(fd);
        string str((const char*)buf);
        if(rev_len>0&&"pedestrian"==str)
            return true;
        else
            return false;
    }

    string get_ip()
    {
        char ip[20];
        memset(ip,0,20);
        char mac[20];
        memset(mac,0,20);
        char mask[20];
        memset(mask,0,20);
        Socket::get_ipaddr(ip,mac,mask);
        return string(ip);
    }
    string get_gateway()
    {
        // Socket::get_gateway()
    }
    void udp_send(string ip,int port,char *data,int len )
    {
        // Socket::SendDataByClient((char *)data,len,ip.data(),port);
        int fd=Socket::UdpCreateSocket(0);
        Socket::UdpSendData(fd,ip.data(),port,data,len);
        close(fd);
    }
};

#endif // SERVER_H
