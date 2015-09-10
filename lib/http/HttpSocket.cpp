#include "HttpSocket.h"

HttpSocket::HttpSocket(TcpSocket s) {
    tcp_socket = s;
}

void HttpSocket::close() {
    tcp_socket.close();
}

void HttpSocket::write(const HttpResponse& response) const {
    tcp_socket.write(response.header.toStdString().data(), response.header.toStdString().size());
    tcp_socket.write(response.getContent(), response.getContentSize());

}

void HttpSocket::read(HttpRequest& request) const {
    request.clear();
    std::string s = "";
    int result = 1;
    while (result == 1) {
        tcp_socket.read(s);
    }
    request = HttpRequest(QString::fromUtf8(s.data(), s.size()));
}
