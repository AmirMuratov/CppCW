#include "HttpServer.h"

HttpServer::HttpServer() {
}

void HttpServer::addPort(int port, std::function<void(HttpRequest&, HttpSocket&)> queryHandler) {
    tcpServer.addPort(port, bind(newRequest, queryHandler, std::placeholders::_1));//TODO
}

void HttpServer::start() {
    tcpServer.start();
}

void HttpServer::stop() {
    tcpServer.stop();
}

void newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack,
                int fd) {
    std::string str;
    TcpSocket s(fd);
    int res = s.read(str);// 0 - read returned 0, -1 - error, 1 - EAGAIN
    if (res < 0) {
        s.close();
        return;
    }
    if (str.length() == 0) {
        return;
    }
    HttpSocket http_s(s);
    HttpRequest req(QString::fromStdString(str));
    callBack(req, http_s);
    if (res == 0) {
        //read returned 0
        s.close();
    }
}


