#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include "TcpSocket.h"
#include <signal.h>
#include <sys/eventfd.h>


class TcpServer {
    static const int EPOLL_RUN_TIMEOUT = 1000;
    static const int MAX_EVENTS = 64;
    static const int MAX_CONNECTION = 10;//will be ignored(linux 2.6.8+)

    bool listening;
    int epfd, listen_socket;

    int createEpollFd();
    int addSocket(int socket);
    int setUpListenSocket(uint16_t port);
    int bindSocket(int socket, uint16_t port);

    void epollLoop();
    int acceptNewConnection();
    void sigactionHandle();
    int makeNonBlocking(int socket);
    std::function<void(const TcpSocket&)> newData;

public:
    TcpServer(std::function<void(const TcpSocket&)>);
    void start(uint16_t port);
    void stop();
};

#endif // TCPSERVER_H
