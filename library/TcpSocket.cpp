#include "TcpSocket.h"

TcpSocket::TcpSocket(TcpServer* serv, int socket) {
    server = serv;
    fd = socket;
}

TcpSocket::~TcpSocket() {
    close();
}

void TcpSocket::close() {
    std::cout << "closing tcp: " << fd << std::endl;
    server->epoll.remove(fd);
    if(server->sockets.count(fd) != 0) {
        server->sockets.remove(fd);
    }
    ::close(fd);
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
            data[size] = '\0';
            str.append(data);
        }
    }
}

int TcpSocket::write(const char * data, size_t size) {
    std::cout << "tcp writing" << std::endl;
    if (fd > -1) {
        int count;
        unsigned int shift = 0;
        do {
            count = ::write(fd, data + shift, size - shift);
            if (count == -1) {
                if (errno != EAGAIN) {
                    printf("writing failed: %s\n", strerror(errno));
                    return -1;
                }
            } else {
                shift += count;
            }
        } while (shift < size);
        return 0;
    } else {
        printf("writing failed, socket error");
        return -1;
    }
}

/*int TcpSocket::write(const char * data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[buffersize + i] = data[i];
    }
    buffersize += size;
    return flush();
}

int TcpSocket::flush() {
    std::cout << "flushing " << buffersize << " bytes" << std::endl;
    if (buffersize < 1000) {
        std::cout << std::string(buffer, buffersize) << std::endl;
    }
    int count = 0;
    while (count != buffersize) {
        int written = ::write(fd, buffer + count, buffersize - count);
        if (written == -1) {
            if (errno != EAGAIN) {
                printf("writing failed: %s\n", strerror(errno));
                return -1;
            } else {
                break;
            }
        } else {
           count += written;
        }
    }
    for (int i = count; i < buffersize; i++) {
        buffer[i - count] = buffer[i];
    }
    buffersize -= count;
    return buffersize;
}*/
