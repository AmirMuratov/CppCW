#include "TcpSocket.h"

TcpSocket::TcpSocket(int socket) {
    fd = socket;
}

TcpSocket::TcpSocket() {

}

void TcpSocket::close() {
    ::close(fd);
}

int TcpSocket::read(std::string& string) const {
    if (fd > -1) {
        char data[BUFFER_SIZE];
        int size;
        while (1) {
            size = ::read(fd, data, BUFFER_SIZE);
            if (size == 0) {
                return 0;
            } else if (size == -1) {
                if (errno != EAGAIN) {
                    printf("reading failed: %s\n", strerror(errno));
                    string = "";
                    return -1;
                } else {
                    return 1;
                }
            } else {
                string += std::string(data, size);
            }
        }
        return 0;
    } else {
        printf("reading failed, socket error");
        return -1;
    }
}

int TcpSocket::write(const char * data, size_t size) const {
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
