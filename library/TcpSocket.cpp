#include "TcpSocket.h"

TcpSocket::TcpSocket(TcpServer* serv, int socket) {
    std::cout << "Creating TCP socket: " << socket << std::endl;
    server = serv;
    fd = socket;
    buffersize = 0;
    closed = false;
}

TcpSocket::~TcpSocket() {
    std::cout << "Destructing socket: " << fd << std::endl;
    close();
}

void TcpSocket::close() {
    if (!closed) {
        std::cout << "closing tcp socket: " << fd << std::endl;
        ::close(fd);
        closed = true;
    }
}

int TcpSocket::read(QString& str) {
    char data[BUFFER_SIZE];
    int size;
    while (1) {
        size = ::read(fd, data, BUFFER_SIZE);
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
            data[size] = 0;
            str.append(data);
        }
    }
}

int TcpSocket::write(const char * data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[buffersize + i] = data[i];
    }
    buffersize += size;
    server->epoll.modify(fd, EPOLLOUT | EPOLLIN | EPOLLET | EPOLLRDHUP);
    return 0;
}

void TcpSocket::flush() {
       std::cout << "flushing " << buffersize << " bytes" << std::endl;
        size_t count = 0;
        while (count != buffersize) {
            int written = ::write(fd, buffer + count, buffersize - count);
            if (written < 0) {
                if (errno != EAGAIN) {
                    std::cout << "writing failed: %s\n" << std::endl;
                    return;
                } else {
                    break;
                }
            } else {
                count += written;
            }
        }
        for (size_t i = count; i < buffersize; i++) {
            buffer[i - count] = buffer[i];
        }
        if (buffersize == count) {
            server->epoll.modify(fd, EPOLLIN | EPOLLET | EPOLLRDHUP);
        }
        std::cout << count << " bytes flushed" << std::endl;
        buffersize -= count;
}
