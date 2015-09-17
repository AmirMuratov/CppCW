#include "FdWrap.h"

FDWrap::FDWrap(int f) {
    fd = f;
    std::cout << "creating socket: " << fd << std::endl;
}

FDWrap::~FDWrap() {
    ::close(fd);
    std::cout << "deleting socket: " << fd << std::endl;
}

FDWrap::operator int() {
    return fd;
}


int FDWrap::getfd() {
    return fd;
}
