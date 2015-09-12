#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QString>
#include "HttpResponse.h"
#include "HttpSocket.h"
#include "HttpRequest.h"
#include "TcpServer.h"
#include <QTextStream>

class HttpServer {
    TcpServer tcpServer;

public:
    HttpServer(std::function<void(HttpRequest&, HttpSocket&)> callBack);
    void start(u_int16_t port);
    void stop();
};

void newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack, const TcpSocket& s);

#endif // HTTPSERVER_H
