#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <thread>
#include <functional>

#include <sys/ioctl.h>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <vector>

#include "tool.h"

#define	TCP_NODELAY	 1

using namespace std;

class Socket{
public:
    Socket();

    static  int close_socket(int *s)
    {
        shutdown(*s,SHUT_RDWR);
        close(*s);
        *s=-1;
    }

    static   int StartTcpServerSock(unsigned short port, int timeoutsec,int maxqueue);

    static    int CreatBroadcast(int port)
    {
        int socket_fd;
        struct sockaddr_in user_addr;

        user_addr.sin_family=AF_INET;
        //    user_addr.sin_family=AF_UNIX;
        user_addr.sin_port=htons(port);
        user_addr.sin_addr.s_addr=htonl(INADDR_ANY);
        if((socket_fd=(socket(AF_INET,SOCK_DGRAM,0)))==-1){
            perror("socket");
            return -1;
        }

        int so_broadcast=1;
        setsockopt(socket_fd,SOL_SOCKET, SO_BROADCAST, &so_broadcast,sizeof(so_broadcast));
        if((bind(socket_fd,(struct sockaddr *)&user_addr,sizeof(struct sockaddr)))==-1){
            perror("bind");
            return -1;
        }


        int ul = 0;
        ioctl(socket_fd, FIONBIO, &ul); //设置为阻塞模式

        return socket_fd;
    }

    static   int RecvFromBroadcast(unsigned char *buffer, int len, int sock)
    {
        struct sockaddr user_addr;
        int leng=sizeof(struct sockaddr_in);// TODO:important
        //  printf("geting broadcast ...");fflush(NULL);
        int rec_len= recvfrom(sock,buffer,len,0,(struct sockaddr *)&user_addr,(socklen_t *)&leng);
        if(rec_len>0){
            struct sockaddr_in *p=( struct sockaddr_in *)&user_addr;
            //inet_ntoa
            printf("get  size %d, from %s %d\n",rec_len,inet_ntoa(p->sin_addr),ntohs(p->sin_port));fflush(NULL);
        }
        return rec_len;
    }
    static   int RecvFromBroadcast(unsigned char *buffer, int len, int sock,string &peer_ip,int &peer_port)
    {
        struct sockaddr user_addr;
        int leng=sizeof(struct sockaddr_in);// TODO:important
        // printf("geting broadcast ...");fflush(NULL);
        int rec_len= recvfrom(sock,buffer,len,0,(struct sockaddr *)&user_addr,(socklen_t *)&leng);
        if(rec_len>0){
            struct sockaddr_in *p=( struct sockaddr_in *)&user_addr;
            peer_ip=string(inet_ntoa(p->sin_addr));
            peer_port=ntohs(p->sin_port);
            //inet_ntoa
            printf("broadcast from %s %d\n",inet_ntoa(p->sin_addr),ntohs(p->sin_port));fflush(NULL);
        }
        return rec_len;
    }
    static   int WaitTcpConnect(int sock, unsigned long sec, char * ip, unsigned short * port)
    {
        int cl_fd=0, one;
        fd_set rd_set;
        struct timeval timeout;
        struct sockaddr_in  client_addr;
        int client_len = sizeof(struct sockaddr_in);

        timeout.tv_sec = sec;
        timeout.tv_usec = 0;

        FD_ZERO(&rd_set);
        FD_SET(sock, &rd_set);
        while ((cl_fd = select(sock + 1, &rd_set, NULL, NULL, &timeout)) < 0) {

            if (EINTR == errno) {
                //prt(err, "socket err EINTR");

            }
            if (EINVAL == errno) {
                //prt(err, "socket err EINVAL");

            }
            if (ENOMEM == errno) {
                //prt(err, "socket err ENOMEM");

            }
            if (EBADF == errno) {
                //prt(err, "socket err EBADF");

            }
            //prt(err, "socket err,need rebind socket");
            cl_fd = -1;
            return cl_fd;
        }

        if(FD_ISSET(sock, &rd_set))
        {
            //	//prt(net,"get msg from client");
#if 0
            while((cl_fd = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_len))<0){
                if(EINTR==errno){
                    //Log0("accept continue: cl_fd[%d] errno:[%s]", cl_fd, strerror(errno));
                    continue;
                }
                else{
                    //Log0("accept error: cl_fd[%d] errno:[%s]", cl_fd, strerror(errno));
                    return 0;
                }
            }
#else
            if((cl_fd = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_len))<=0){

                //prt(err,"accept  err  %d",cl_fd);
                close_socket(&sock);

                cl_fd=-1;
            }else{
                //		  	//prt(net,"accept rst %d",cl_fd);
            }
#endif
#if 0
#if 0
            one = 1;
            if(setsockopt(cl_fd, IPPROTO_TCP, TCP_NODELAY,
                          (char *)&one, sizeof(one))<0){
                close(cl_fd);
                //Log0("setsockopt error: cl_fd[%d]", cl_fd);
                return 1;
            }
#else
            //设置发送超时
            //设置接收超时
            struct timeval timeout1 = {0,450000};
            if((setsockopt(cl_fd, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout1, sizeof(struct timeval))<0)
                    || (setsockopt(cl_fd, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout1, sizeof(struct timeval))<0)){
                close(cl_fd);
                //Log0("setsockopt error: cl_fd[%d] errno:[%s]", cl_fd, strerror(errno));
                return 1;
            }
#endif
#endif

            if(ip != NULL)
                strcpy(ip, inet_ntoa(client_addr.sin_addr));
            if(port != NULL)
                *port = ntohs(client_addr.sin_port);
            // return cl_fd;
        }else
        {
            //prt(net,"client time out ,returning %d ",cl_fd);
        }
        return cl_fd;
        //   return 1;
    }

    static   int ConnectTcpClient(int sock,const  char * ip, unsigned short port)
    {
        struct sockaddr_in server_addr;
        memset(&server_addr, 0 , sizeof(struct sockaddr_in));
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

#if 0
        int error=-1, len, one;
        len = sizeof(int);
        timeval tm;
        fd_set set;
        unsigned long ul = 1;
        ioctl(sock, FIONBIO, &ul); //设置为非阻塞模式
#endif



        fd_set set;
        unsigned long ul = 1;
        ioctl(sock, FIONBIO, &ul); //设置为非阻塞模式

        ul = 0;
        ioctl(sock, FIONBIO, &ul); //设置为阻塞模式

        //	int save_mode = fcntl(sock, F_GETFL, 0);
        //	fcntl(sock, F_SETFL, save_mode | O_NONBLOCK);//设置为阻塞模式

        if(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
            {
                //    		ul = 0;
                //    	 	ioctl(sock, FIONBIO, &ul); //设置为 阻塞模式

                return -1;
            }
#if 0
            tm.tv_sec = 1;
            tm.tv_usec = 0;
            FD_ZERO(&set);
            FD_SET(sock, &set);
            if(select(sock+1, NULL, &set, NULL, &tm)>0){
                getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
                if(error != 0)
                    return -1;
            } else
                return -1;
#if 0
            one = 1;
            if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                          (char *)&one, sizeof(one))<0){
                close(sock);
                return 1;
            }
#endif
#endif
        }

#if 0
        ul = 0;
        ioctl(sock, FIONBIO, &ul); //设置为阻塞模式
#endif
        return 1;
    }

    static   int RecvDataByTcp(int sock,char* buffer,int len)
    {
        //prt(info,"get buffer len : %d",len);
        int offset=0;
        while(len>0){
            int re = recv(sock,buffer+offset,len,0);
            if(re<=0){
                if(re<0 && EINTR == errno){
                    //Log0("recv continue: re[%d] errno:[%s]", re, strerror(errno));
                    continue;
                }
                else{
                    //Log0("recv error: re[%d] errno:[%s]", re, strerror(errno));
                    return offset;
                }
            }
            len -= re;
            offset += re;
        }
        return offset;
    }

    static   int RecvDataByTcp1(int sock,char* buffer,int len)
    {
        //prt(info,"get buffer len : %d",len);
        int offset=0;
        while(len>0){
            int re = recv(sock,buffer+offset,len,0);
            if(re<=0){
                if(re<0 && EINTR == errno){
                    //Log0("recv continue: re[%d] errno:[%s]", re, strerror(errno));
                    continue;
                }
                else{
                    //Log0("recv error: re[%d] errno:[%s]", re, strerror(errno));
                    return offset;
                }
            }
            offset += re;
            return offset;
            //        len -= re;
            //        offset += re;
        }
        return offset;
    }

    //int RecvDataByTcp(int sock,char* buffer,int len){
    //int re = recv(sock,buffer+offset,len,0);

    //}
    static    int SendDataByTcp(int sock,const char * buffer, int len)
    {
        //	//prt(info,"send buffer len : %d",len);
        int ret = send(sock, buffer, len, 0);
        if(ret<=0){
            return -1;
        }
        return ret;
    }

#if 0
    int SendDataByTcp(int sock, char * buffer, int len)
    {
        int ret,lens = 0;
        while(1){
            ret = send(sock, buffer+lens, len-lens, 0);
            if(ret<=0){
                if(ret<0 && EINTR == errno){
                    //Log0("send continue: re[%d] errno:[%s]", ret, strerror(errno));
                    continue;
                }
                else{
                    //Log0("send error: ret[%d] errno:[%s]", ret, strerror(errno));
                    return ret;
                }
            }
            else{
                lens += ret;
                if(lens == len)
                    break;
            }
        }
        return lens;
    }
#endif

    static    int SendDataByClient(const char*buffer, int len,const char* ip, int port)
    {
        int sc = CreateTcpClientSock(0, 1);
        if(sc==-1)
            return sc;

        int ret = ConnectTcpClient(sc, ip, port);
        if(ret==-1)
        {
            close(sc);
            return -1;
        }

        ret = SendDataByTcp(sc, buffer, len);
        close(sc);
        return ret;
    }

    static    int CreateTcpClientSock(unsigned short port, int timeoutsec)
    {
        int fd;
        int value;
        struct timeval timeo;
        socklen_t len = sizeof(timeo);

        memset(&timeo, 0, sizeof(timeo));
        if((fd =socket(AF_INET, SOCK_STREAM, 0))<0)
            return -1;

        value = 1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value , sizeof(value))<0){
            close(fd);
            return -1;
        }

        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(struct sockaddr_in));
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = INADDR_ANY;
        sa.sin_port = htons(port);
        if(bind(fd, (struct sockaddr *)(&sa), sizeof(struct sockaddr))<0){

            close(fd);
            return -1;
        }
        return fd;
    }

    static   int UdpCreateSocket(unsigned short port)
    {
        int sockfd;
        struct sockaddr_in my_addr;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
            return -1;
        }

        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))<0){
            // print_net("bind port %d err",port);
            return -1;
        }else{
            // print_net("bind port %d ok",port);
        }
        return sockfd;
    }

    static   int UdpSendData(int sock, const char * ip, unsigned short port,const  char * buf, int slen)
    {
        struct sockaddr_in their_addr;
        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &their_addr.sin_addr);
        int ret,sended = 0;
        int len = slen;
        while(1){
            ret = sendto(sock, buf+sended, len, 0,(struct sockaddr*)&their_addr, sizeof(struct sockaddr));
            if(ret == -1){
                break;
            }
            else{
                sended += ret;
                len    -= ret;
                if(sended == slen)break;
            }
        }
        return sended;
    }
    static   int RecvDataByUdp(int sock,char* buffer,int len)
    {
        //prt(info,"get buffer len : %d",len);
        int offset=0;
        while(len>0){
            int re = recv(sock,buffer+offset,len,0);
            if(re<=0){
                if(re<0 && EINTR == errno){
                    //Log0("recv continue: re[%d] errno:[%s]", re, strerror(errno));
                    continue;
                }
                else{
                    //Log0("recv error: re[%d] errno:[%s]", re, strerror(errno));
                    return offset;
                }
            }
            offset += re;
            return offset;
            //        len -= re;
            //        offset += re;
        }
        return offset;
    }
    static void hex2str(char  *pbDest, char *pbSrc, int nLen)
    {
        char ddl, ddh;
        int i;

        for (i = 0; i < nLen; i++) {
            ddh = 48 + pbSrc[i] / 16;
            ddl = 48 + pbSrc[i] % 16;
            if (ddh > 57)
                ddh = ddh + 7;
            if (ddl > 57)
                ddl = ddl + 7;
            pbDest[i * 2] = ddh;
            pbDest[i * 2 + 1] = ddl;
        }

        pbDest[nLen * 2] = '\0';
    }
#define IF_NAMESIZE	16

#define BUFSIZE 8192
    struct route_info{
        u_int dstAddr;
        u_int srcAddr;
        u_int gateWay;
        char ifName[IF_NAMESIZE];
    };
    static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)
    {
        struct rtmsg *rtMsg;
        struct rtattr *rtAttr;
        int rtLen;
        char *tempBuf = NULL;
        struct in_addr dst;
        struct in_addr gate;

        tempBuf = (char *)malloc(100);
        rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
        // If the route is not for AF_INET or does not belong to main routing table
        //then return.
        if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
            return;

        rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
        rtLen = RTM_PAYLOAD(nlHdr);
        for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
            switch(rtAttr->rta_type) {
            case RTA_OIF:
                if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
                break;
            case RTA_GATEWAY:
                rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
                break;
            case RTA_PREFSRC:
                rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
                break;
            case RTA_DST:
                rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
                break;
            }
        }
        dst.s_addr = rtInfo->dstAddr;
        if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
        {
            printf("oif:%s\n",rtInfo->ifName);
            gate.s_addr = rtInfo->gateWay;
            sprintf(gateway, (char *)inet_ntoa(gate));
            printf("%s\n",gateway);
            gate.s_addr = rtInfo->srcAddr;
            printf("src:%s\n",(char *)inet_ntoa(gate));
            gate.s_addr = rtInfo->dstAddr;
            printf("dst:%s\n",(char *)inet_ntoa(gate));
        }
        free(tempBuf);
        return;
    }
    static  void get_ipaddr_wlan(char *ipaddr,char *mac,char *mask,const char *lan_ip)
    {
        int tmp_fd;
        int ifs;
        char *ip_str;
        char *mask_str;
        char *mac_str;
        struct ifconf ifconfig;
        struct ifreq if_buf[16];
        ifconfig.ifc_len = sizeof(if_buf);
        ifconfig.ifc_buf = (caddr_t) if_buf;

        if ((tmp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket(AF_INET, SOCK_DGRAM, 0)");
        }
        if (ioctl(tmp_fd, SIOCGIFCONF, (char *) &ifconfig) == -1) {
            perror("SIOCGIFCONF ioctl");
        }
        ifs = ifconfig.ifc_len / sizeof(struct ifreq);
        while (ifs-- > 0) {
            //	printf("1");
            if (!(ioctl(tmp_fd, SIOCGIFFLAGS, (char *) &if_buf[ifs]))) {
                //	if (buf[if_len].ifr_flags & IFF_UP) {//check out whether network is down or not
                if (1) {
                    // printf("status: UP\n");

                    if (!(ioctl(tmp_fd, SIOCGIFADDR, (char *) &if_buf[ifs]))) {
                        ip_str =(char*) inet_ntoa(((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr);
                        //					printf("addr:%s\n",
                        //							ip_str =
                        //									(char*) inet_ntoa(
                        //											((struct sockaddr_in*) (&buf[ifs].ifr_addr))->sin_addr));
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s", if_buf[ifs].ifr_name);
                        perror(str);
                    }
                    if (!strcmp(ip_str, "127.0.0.1")) {
                        continue;
                    }
                    prt(info:"local ip %s\n", ip_str);
                    memcpy(ipaddr,ip_str,16);


                    if (!(ioctl(tmp_fd, SIOCGIFNETMASK, (char *) &if_buf[ifs]))) {
                        mask_str=	(char*) inet_ntoa(
                                    ((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr);


                        prt(info:"local mask %s\n", mask_str);
                        memcpy(mask,mask_str,16);
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s",
                                if_buf[ifs].ifr_name);
                        perror(str);
                    }

                    if (!(ioctl(tmp_fd, SIOCGIFBRDADDR, (char *) &if_buf[ifs]))) {
                        printf("broadcast:%s\n",
                               (char*) inet_ntoa(
                                   ((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr));
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s", if_buf[ifs].ifr_name);
                        perror(str);
                    }
                    if (!(ioctl(tmp_fd, SIOCGIFHWADDR, (char *) &if_buf[ifs]))) {
                        printf("mac addr:%x:%x:%x:%x:%x:%x\n",
                               (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[0],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[1],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[2],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[3],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[4],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[5]);


                        mac_str=(char *)if_buf[ifs].ifr_hwaddr.sa_data;
                        hex2str(mac,mac_str,6);
                        sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[0],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[1],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[2],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[3],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[4],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[5]);
                        //memcpy(mac,mac_str,14);
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFHWADDR ioctl %s",
                                if_buf[ifs].ifr_name);
                        perror(str);
                    }

                    if(ip_in_lan(ipaddr,lan_ip))
                        break;
                } else {
                    printf("this interface is DOWN\n");
                }
            }
        }
    }
    inline static string sub_same_string(string src,const string dst)
    {
        int i;
        for( i=0;i<src.size()&&i<dst.size();i++){
            if(src[i]!=dst[i])
                break;
        }
        string sub=src.substr(0,i);
        return sub;
    }
    static  bool ip_in_lan(char *src,const char *dst)
    {
        bool ret=false;
        string s(src);
        string d(dst);
        string same=sub_same_string(d,s);
        if(same.size()>0){
            int flg=0;
            for(int i=0;i<same.size();i++){
                if(same[i]=='.')flg++;
            }
            if(flg==3)
                ret=true;
        }
        return ret;
    }
    static  void get_ipaddr(char *ipaddr,char *mac,char *mask)
    {
        int tmp_fd;
        int ifs;
        char *ip_str;
        char *mask_str;
        char *mac_str;
        struct ifconf ifconfig;
        struct ifreq if_buf[16];
        ifconfig.ifc_len = sizeof(if_buf);
        ifconfig.ifc_buf = (caddr_t) if_buf;

        if ((tmp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket(AF_INET, SOCK_DGRAM, 0)");
        }
        if (ioctl(tmp_fd, SIOCGIFCONF, (char *) &ifconfig) == -1) {
            perror("SIOCGIFCONF ioctl");
        }
        ifs = ifconfig.ifc_len / sizeof(struct ifreq);
        while (ifs-- > 0) {
            //	printf("1");
            if (!(ioctl(tmp_fd, SIOCGIFFLAGS, (char *) &if_buf[ifs]))) {

                //	if (buf[if_len].ifr_flags & IFF_UP) {
                if (1) {
                    // printf("status: UP\n");

                    if (!(ioctl(tmp_fd, SIOCGIFADDR, (char *) &if_buf[ifs]))) {
                        ip_str =(char*) inet_ntoa(((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr);
                        //					printf("addr:%s\n",
                        //							ip_str =
                        //									(char*) inet_ntoa(
                        //											((struct sockaddr_in*) (&buf[ifs].ifr_addr))->sin_addr));
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s", if_buf[ifs].ifr_name);
                        perror(str);
                    }
                    if (!strcmp(ip_str, "127.0.0.1")) {
                        continue;
                    }
                    prt(info:"local ip %s\n", ip_str);
                    memcpy(ipaddr,ip_str,16);


                    if (!(ioctl(tmp_fd, SIOCGIFNETMASK, (char *) &if_buf[ifs]))) {
                        mask_str=	(char*) inet_ntoa(
                                    ((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr);


                        prt(info:"local mask %s\n", mask_str);
                        memcpy(mask,mask_str,16);
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s",
                                if_buf[ifs].ifr_name);
                        perror(str);
                    }



                    if (!(ioctl(tmp_fd, SIOCGIFBRDADDR, (char *) &if_buf[ifs]))) {
                        printf("broadcast:%s\n",
                               (char*) inet_ntoa(
                                   ((struct sockaddr_in*) (&if_buf[ifs].ifr_addr))->sin_addr));
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFADDR ioctl %s", if_buf[ifs].ifr_name);
                        perror(str);
                    }
                    if (!(ioctl(tmp_fd, SIOCGIFHWADDR, (char *) &if_buf[ifs]))) {
                        printf("mac addr:%x:%x:%x:%x:%x:%x\n",
                               (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[0],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[1],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[2],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[3],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[4],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[5]);


                        mac_str=(char *)if_buf[ifs].ifr_hwaddr.sa_data;
                        hex2str(mac,mac_str,6);
                        sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[0],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[1],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[2],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[3],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[4],
                                (unsigned char) if_buf[ifs].ifr_hwaddr.sa_data[5]);
                        //memcpy(mac,mac_str,14);
                    } else {
                        char str[256];
                        sprintf(str, "SIOCGIFHWADDR ioctl %s",
                                if_buf[ifs].ifr_name);
                        perror(str);
                    }
                } else {
                    // printf("status: DOWN\n");
                }
            }
        }
    }
    static  int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
    {
        struct nlmsghdr *nlHdr;
        int readLen = 0, msgLen = 0;
        do{

            if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
            {
                perror("SOCK READ: ");
                return -1;
            }

            nlHdr = (struct nlmsghdr *)bufPtr;

            if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
            {
                perror("Error in recieved packet");
                return -1;
            }


            if(nlHdr->nlmsg_type == NLMSG_DONE)
            {
                break;
            }
            else
            {

                bufPtr += readLen;
                msgLen += readLen;
            }


            if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
            {

                break;
            }
        } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
        return msgLen;
    }


    static  int get_gateway(char *gateway)
    {
        struct nlmsghdr *nlMsg;
        struct rtmsg *rtMsg;
        struct route_info *rtInfo;
        char msgBuf[BUFSIZE];

        int sock, len, msgSeq = 0;

        if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
        {
            perror("Socket Creation: ");
            return -1;
        }


        memset(msgBuf, 0, BUFSIZE);


        nlMsg = (struct nlmsghdr *)msgBuf;
        rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);


        nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
        nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

        nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
        nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
        nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.


        if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
            printf("Write To Socket Failed��\n");
            return -1;
        }


        if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
            printf("Read From Socket Failed��\n");
            return -1;
        }

        rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
        for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
            memset(rtInfo, 0, sizeof(struct route_info));
            parseRoutes(nlMsg, rtInfo,gateway);
        }
        free(rtInfo);
        close(sock);
        return 0;
    }



};
#endif // SOCKET_H
