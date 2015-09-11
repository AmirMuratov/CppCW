#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "TcpSocket.h"


class TcpServer {
     std::function<void(const TcpSocket&)> dataAvailable;
     bool running;

public:
    TcpServer(std::function<void(const TcpSocket&)>);
    void start(int port);
    void stop();
};


#endif // TCPSERVER_H
