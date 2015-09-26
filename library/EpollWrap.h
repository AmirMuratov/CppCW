#ifndef EPOLLWRAP_H
#define EPOLLWRAP_H

#include <QMap>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>

class EpollWrap {
    int const MAX_EVENTS = 50;
    bool running;
    int epollfd;
    QMap<int, std::function<void(__uint32_t)>> callbacks;

public:

    EpollWrap();
    void startListening();
    void stopListening();
    ~EpollWrap();
    int add(int, std::function<void(__uint32_t)>, __uint32_t); //file descriptor to add, function to call when event
                                                                    //happens(gets fd and events), and events.
    int remove(int);
    int modify(int, __uint32_t);
    bool isListening(int);
};

#endif // EPOLLWRAP_H
