#include "EpollWrap.h"

#define MAX_EVENTS 50

EpollWrap::EpollWrap() {
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        printf("cant create epoll instance\n");
    }
    std::cout << "Creating epoll wrap " << epollfd << std::endl;
}


void EpollWrap::startListening() {
    std::cout << "Epoll starts listening." << std::endl;
    running = true;
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int n;
        n = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (!running) {
            break;
        }
        std::cout << "EPOLL EVENTS: " << n << std::endl;
        std::cout << "==============================" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << "descriptor: " << events[i].data.fd << " events:" << std::endl;
            if (events[i].events & EPOLLIN) {
                std::cout << "EPOLLIN" << std::endl;
            }
            if (events[i].events & EPOLLOUT) {
                std::cout << "EPOLLOUT" << std::endl;
            }
            if (events[i].events & EPOLLERR) {
                std::cout << "EPOLLERR" << std::endl;
            }
            if (events[i].events & EPOLLRDHUP) {
                std::cout << "EPOLLRDHUP" << std::endl;
            }
            callbacks[events[i].data.fd](events[i].events);
        }
        std::cout << "==============================" << std::endl;
    }
}

void EpollWrap::stopListening() {
    running = false;
}

EpollWrap::~EpollWrap() {
    std::cout << "Deleting epoll wrap " << epollfd << std::endl;
    for (auto i = callbacks.begin(); i != callbacks.end(); i++) {
        std::cout << "closing " << i.key() << " descriptor." << std::endl;
    }
    if (epollfd != -1) {
        close(epollfd);
    }
}

int EpollWrap::add(int fd, std::function<void(__uint32_t)> callback, __uint32_t events) {
    std::cout << "Added " << fd << " descriptor to epoll." << std::endl;
    struct epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    callbacks.insert(fd, callback);
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1){
        return -1;
    }
    return 0;
}

int EpollWrap::remove(int fd) {
    std::cout << "removing from epoll: " << fd << std::endl;
    if (callbacks.count(fd) != 0) {
        callbacks.remove(fd);
    }
    struct epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event) == -1) {
        return -1;
    }
    return 0;
}

int EpollWrap::modify(int fd, __uint32_t events) {
    std::cout << "modifying " << fd << " socket" << std::endl;
    struct epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) == -1) {
        return -1;
    }
    return 0;

}

bool EpollWrap::isListening(int fd) {
    return callbacks.count(fd) == 1;
}

