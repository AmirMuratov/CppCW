#include "HttpServer.h"

HttpServer::HttpServer() {
}

int HttpServer::addPort(int port, std::function<void(HttpRequest&, HttpSocket&)> queryHandler) {
    return tcpServer.addPort(port, bind(&HttpServer::newRequest, this, queryHandler, std::placeholders::_1, std::placeholders::_2));
}

void HttpServer::start() {
    tcpServer.start();
}

void HttpServer::stop() {
    tcpServer.stop();
}

void HttpServer::newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack,
                TcpSocket* socket, EventType type) {
    if (type == ERROR || type == HUP) {
        requests.remove(socket);
        return;
    }
    if (type == NEWDATA) {
        QString str;
        int res = socket->read(str);// 0 - read returned 0, -1 - error, 1 - EAGAIN
        if (res < 0) {
            if (requests.count(socket) != 0) {
                requests.remove(socket);
            }
            return;
        }
        if (requests.count(socket) == 0) {
            requests[socket] = HttpRequest();
        }
        //std::cout << str.toStdString()<< std::endl;
        requests[socket].append(str);
        if (requests[socket].isValid()) {
            std::cout << "new Request" << std::endl;
            HttpSocket http_s(socket);
            callBack(requests[socket], http_s);
            requests.remove(socket);
        }
        if (res == 0) {
            requests.remove(socket);
        }
    }
}


