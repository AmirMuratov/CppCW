#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include "FdWrap.h"
#include "EpollWrap.h"

//typedef std::shared_ptr<FDWrap> fdptr;

class TcpServer {

    EpollWrap epoll;
    static int makeSocketNonBlocking(int);
    static int createAndBind(int);//gets port, returns fd
    static void connectionHandler(EpollWrap*, std::function<void(int)>, int, __uint32_t);
    static void dataHandler(std::function<void(int)>, int, __uint32_t);



public:
    TcpServer();
    int addPort(int, std::function<void(int)>); //port, new data callback
    void start();
    void stop();
};


#endif // TCPSERVER_H
