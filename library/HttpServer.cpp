#include "HttpServer.h"

HttpServer::HttpServer(std::function<void(HttpRequest&, HttpSocket&)> callBack):
    tcpServer(std::bind(newRequest, callBack, std::placeholders::_1)) {
}

void newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack, const TcpSocket& s) {
    std::string str;
    s.read(str);
    HttpSocket http_s(s);
    HttpRequest req(QString::fromStdString(str));
    callBack(req, http_s);
}

void HttpServer::start(uint16_t port) {
    tcpServer.start(port);
}

void HttpServer::stop() {
    tcpServer.stop();
}

