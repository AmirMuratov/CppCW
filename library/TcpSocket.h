#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <QString>
#include <memory>
class TcpSocket;
#include "TcpServer.h"

typedef std::shared_ptr<TcpSocket> sockptr;

class TcpSocket {
    static const int BUFFER_SIZE = 100000;
    int fd;
    TcpServer* server;
    size_t buffersize;
    char buffer[BUFFER_SIZE];
    bool closed;

public:

    TcpSocket(TcpServer*, int);
    ~TcpSocket();
    void close();
    int read(QString&);
    int write(const char * data, size_t size);
    void flush();

    friend class TcpServer;
};

#endif // TCPSOCKET_H
