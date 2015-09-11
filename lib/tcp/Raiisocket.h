#ifndef RAIISOCKET_H
#define RAIISOCKET_H

#include <unistd.h>

class RAIISocket
{
    int fd;
public:
    RAIISocket(int);
    ~RAIISocket();
    operator int() const;
};

#endif // RAIISOCKET_H
