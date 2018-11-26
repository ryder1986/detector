#include "socket.h"




Socket::Socket(){}

int Socket::StartTcpServerSock(unsigned short port, int timeoutsec, int maxqueue)
{
    int fd;
    int value;
    struct timeval timeo;
    socklen_t len = sizeof(timeo);

    memset(&timeo, 0, sizeof(timeo));
    if((fd= socket(AF_INET,SOCK_STREAM,0)) < 0){
        //prt(err,"socket err");
        return -1;
    }
    value = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value))<0){
        //prt(err,"socket err");
        close(fd);
        return -1;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(port);
    if(bind(fd, (struct sockaddr *)(&sa),sizeof(struct sockaddr))<0){
        //prt(err,"socket err,bind err");
        close(fd);
        return -1;
    }

    if(listen(fd, maxqueue)<0)
    {
        //prt(err,"socket err");
        close(fd);
        return -1;
    }
    return fd;
}
