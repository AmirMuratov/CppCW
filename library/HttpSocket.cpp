#include "HttpSocket.h"

 HttpSocket::HttpSocket(TcpSocket* s) {
    tcp_socket = s;
}

void HttpSocket::close() {
    //tcp_socket->close();
}

void HttpSocket::write(const HttpResponse& response) {
    std::cout << "sending" << std::endl;
    //std::cout << tcp_socket->fd << std::endl;
    tcp_socket->write(response.header.toStdString().data(), response.header.toStdString().size());
    std::cout << "LITTLE" << std::endl;
    tcp_socket->write(response.getContent(), response.getContentSize());
}
