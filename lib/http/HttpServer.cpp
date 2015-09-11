#include "HttpServer.h"

//std::function<void(HttpRequest&, HttpSocket&)> callBack2;
void (*f)(HttpRequest&, HttpSocket&);

void newRequest(const TcpSocket& s) {
    std::string str;
    s.read(str);
    HttpSocket http_s(s);
    HttpRequest req(QString::fromStdString(str));
    f(req, http_s);
    //callBack2(req, http_s);
}

/*HttpServer::HttpServer(std::function<void(HttpRequest&, HttpSocket&)> callBack): tcpServer(newRequest) {
    callBack2 = callBack;
}*/

HttpServer::HttpServer(void (*callBack)(HttpRequest&, HttpSocket&)): tcpServer(newRequest) {
    f = callBack;
}


void HttpServer::start(uint16_t port) {
    tcpServer.start(port);
}

void HttpServer::stop() {
    tcpServer.stop();
}

HttpServer::~HttpServer() {
}
