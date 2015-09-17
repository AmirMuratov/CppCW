#ifndef FDWRAP_H
#define FDWRAP_H

#include <unistd.h>
#include <iostream>

class FDWrap
{
    int fd;
public:
    FDWrap(int);
    ~FDWrap();
    int getfd();
    operator int();
};

#endif // FDWRAP_H
