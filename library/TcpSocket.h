#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <QString>
class TcpSocket;
#include "TcpServer.h"

class TcpSocket {
    int fd;
    TcpServer* server;
    static const int BUFFER_SIZE = 1000000;
    int buffersize;
    char buffer[BUFFER_SIZE];

public:
    TcpSocket(TcpServer*, int);
    ~TcpSocket();
    void close();
    int read(QString&);
    int write(const char * data, size_t size);
    int flush();
};

#endif // TCPSOCKET_H
