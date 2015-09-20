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
class TcpServer;
#include "TcpSocket.h"

//typedef std::shared_ptr<FDWrap> fdptr;

class TcpServer {
    int tcpfd;
    EpollWrap epoll;
    QMap<int, TcpSocket*> sockets;
    static int makeSocketNonBlocking(int);
    static int createAndBind(int);//gets port, returns fd
    static void connectionHandler(TcpServer*, std::function<void(TcpSocket*)>, int, __uint32_t);
    static void dataHandler(TcpServer*, std::function<void(TcpSocket*)>, int, __uint32_t);


public:
    TcpServer();
    int addPort(int, std::function<void(TcpSocket*)>); //port, new data callback
    void start();
    void stop();
friend class TcpSocket;
};


#endif // TCPSERVER_H
