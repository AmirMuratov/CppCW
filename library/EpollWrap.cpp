#include "EpollWrap.h"

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
            callbacks[events[i].data.fd](events[i].data.fd, events[i].events);
        }
    }
}

void EpollWrap::stopListening() {
    running = false;
}

EpollWrap::~EpollWrap() {
    std::cout << "Deleting epoll wrap " << epollfd << std::endl;
    for (auto i = callbacks.begin(); i != callbacks.end(); i++) {
        std::cout << "closing " << i.key() << " descriptor." << std::endl;
        close(i.key());
    }
    if (epollfd != -1) {
        close(epollfd);
    }
}

int EpollWrap::add(int fd, std::function<void(int, __uint32_t)> callback, __uint32_t events) {

    std::cout << "Added " << fd << " descriptor to epoll." << std::endl;
    struct epoll_event event;
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
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event) == -1) {
        return -1;
    }
    return 0;
}

bool EpollWrap::isListening(int fd) {
    return callbacks.count(fd) == 1;
}

