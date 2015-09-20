#include "TcpSocket.h"

TcpSocket::TcpSocket(TcpServer* serv, int socket) {
    std::cout << "Creating TCP socket: " << socket << std::endl;
    server = serv;
    infd = socket;
    outfd = dup(socket);
    buffersize = 0;
}

TcpSocket::~TcpSocket() {
    std::cout << "Destructing socket: " << infd << std::endl;
    close();
}

void TcpSocket::close() {
    std::cout << "closing tcp: " << infd << std::endl;
    server->epoll.remove(infd);
    server->epoll.remove(outfd);
    ::close(infd);
    ::close(outfd);
}

int TcpSocket::read(QString& str) {
    char data[BUFFER_SIZE];
    int size;
    while (1) {
        size = ::read(infd, data, BUFFER_SIZE);
        std::cout << size << std::endl;
        if (size == 0) {
            return 0;
        } else if (size == -1) {
            if (errno != EAGAIN) {
                printf("reading failed: %s\n", strerror(errno));
                str = "";
                return -1;
            } else {
                return 1;
            }
        } else {
            data[size] = '\0';
            str.append(data);
        }
    }
}

int TcpSocket::write(const char * data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[buffersize + i] = data[i];
    }
    buffersize += size;
    if (!server->epoll.isListening(outfd)) {
        std::function<void(int, __uint32_t)> callBack = bind(TcpSocket::flush, this, std::placeholders::_1, std::placeholders::_2);
        server->epoll.add(outfd, callBack, EPOLLOUT | EPOLLET | EPOLLRDHUP);
    }
    return 0;
}

void TcpSocket::flush(TcpSocket* socket, int fd, __uint32_t events) {
    if ((events & EPOLLHUP) || (events & EPOLLERR)) {
        //TODO
    }
    if (events & EPOLLOUT) {
        std::cout << "flushing " << socket->buffersize << " bytes" << std::endl;
        size_t count = 0;
        while (count != socket->buffersize) {
            int written = ::write(socket->outfd, socket->buffer + count, socket->buffersize - count);
            if (written < 0) {
                if (errno != EAGAIN) {
                    printf("writing failed: %s\n", strerror(errno));
                    //TODO
                    return;
                } else {
                    break;
                }
            } else {
                count += written;
            }
        }
        for (size_t i = count; i < socket->buffersize; i++) {
            socket->buffer[i - count] = socket->buffer[i];
        }
        if (socket->buffersize == count) {
            socket->server->epoll.remove(socket->outfd);
        }
        std::cout << count << " bytes flushed" << std::endl;
        socket->buffersize -= count;
    }
}
