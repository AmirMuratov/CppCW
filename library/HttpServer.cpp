#include "HttpServer.h"

HttpServer::HttpServer() {
}

int HttpServer::addPort(int port, std::function<void(HttpRequest&, HttpSocket&)> queryHandler) {
    return tcpServer.addPort(port, bind(newRequest, this, queryHandler, std::placeholders::_1));
}

void HttpServer::start() {
    tcpServer.start();
}

void HttpServer::stop() {
    tcpServer.stop();
}

void HttpServer::newRequest(HttpServer* server, std::function<void(HttpRequest&, HttpSocket&)> callBack,
                TcpSocket* socket) {
    QString str;
    int res = socket->read(str);// 0 - read returned 0, -1 - error, 1 - EAGAIN
    if (res < 0) {
        if (server->requests.count(socket) != 0) {
            server->requests.remove(socket);
        }
        return;
    }
    if (server->requests.count(socket) == 0) {
        server->requests[socket] = HttpRequest();
    }
    //std::cout << str.toStdString()<< std::endl;
    server->requests[socket].append(str);
    if (server->requests[socket].isValid()) {
        std::cout << "new Request" << std::endl;
        HttpSocket http_s(socket);
        callBack(server->requests[socket], http_s);
        server->requests.remove(socket);
    }
    if (res == 0) {
        if (server->requests.count(socket) != 0) {
            server->requests.remove(socket);
        }
    }
}


