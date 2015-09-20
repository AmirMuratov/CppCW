#ifndef HTTPSOCKET_H
#define HTTPSOCKET_H

#include "TcpSocket.h"
#include "HttpResponse.h"
#include <QString>

class HttpSocket {
    TcpSocket* tcp_socket;
public:
    HttpSocket(TcpSocket*);
    void write(const HttpResponse&);
};

#endif // HTTPSOCKET_H
