#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QString>
#include "HttpResponse.h"
#include "HttpSocket.h"
#include "HttpRequest.h"
#include "TcpServer.h"
#include <QTextStream>

class HttpServer {
    TcpServer tcpServer;
    //QMap<int,

public:
    HttpServer();
    void addPort(int, std::function<void(HttpRequest&, HttpSocket&)>);
    void start();
    void stop();
};

void newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack, int s);

#endif // HTTPSERVER_H
