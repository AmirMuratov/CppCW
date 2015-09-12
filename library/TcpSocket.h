#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <QString>
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
#include <fcntl.h>

class TcpSocket {
private:
    int fd;
    static const int BUFFER_SIZE = 1024;
public:
    TcpSocket(int socket);
    TcpSocket();
    void close();
    int read(std::string&) const;
    int write(const char * data, size_t size) const;
};

#endif // TCPSOCKET_H
