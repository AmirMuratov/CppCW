#include "Raiisocket.h"

RAIISocket::RAIISocket(int socket) {
    fd = socket;
}

RAIISocket::~RAIISocket() {
    close(fd);
}

RAIISocket::operator int() const {
    return fd;
}




//RAIISocket::operator =(int) {

//}
