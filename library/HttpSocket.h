#ifndef HTTPSOCKET_H
#define HTTPSOCKET_H

#include "TcpSocket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <QString>

class HttpSocket {
    TcpSocket tcp_socket;
public:
    HttpSocket(TcpSocket);
    void close();
    void write(const HttpResponse&) const;
    void read(HttpRequest&) const;
};

#endif // HTTPSOCKET_H
