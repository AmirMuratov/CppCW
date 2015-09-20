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
    QMap<TcpSocket*, HttpRequest> requests;
    static void newRequest(HttpServer*, std::function<void(HttpRequest&, HttpSocket&)>, TcpSocket*);


public:
    HttpServer();
    int addPort(int, std::function<void(HttpRequest&, HttpSocket&)>);
    void start();
    void stop();
};


#endif // HTTPSERVER_H
