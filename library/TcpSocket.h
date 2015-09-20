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
    static const int BUFFER_SIZE = 5000000;
    int infd;
    int outfd;
    TcpServer* server;

public:

    size_t buffersize;
    char buffer[BUFFER_SIZE];

    TcpSocket(TcpServer*, int);
    ~TcpSocket();
    void close();
    int read(QString&);
    int write(const char * data, size_t size);
    static void flush(TcpSocket*, int, __uint32_t);
};

#endif // TCPSOCKET_H
