#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QString>
#include "HttpResponse.h"
#include "HttpSocket.h"
#include "HttpRequest.h"
#include "TcpServer.h"
#include <QTextStream>

class HttpServer {
    QMap<TcpSocket*, HttpRequest> requests;
    TcpServer tcpServer;
    void newRequest(std::function<void(HttpRequest&, HttpSocket&)>, TcpSocket*, EventType);


public:
    HttpServer();
    int addPort(int, std::function<void(HttpRequest&, HttpSocket&)>);
    void start();
    void stop();
};


#endif // HTTPSERVER_H
