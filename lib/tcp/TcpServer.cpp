#include "TcpServer.h"
#include <sys/eventfd.h>
#include "fcntl.h"

TcpServer::TcpServer(std::function<void(const TcpSocket&)> callBack) {
    newData = callBack;
    listening = false;
}


void TcpServer::start(uint16_t port) {
    listening = true;
    if (createEpollFd() == -1) {
        stop();
        return;
    }
    if (setUpListenSocket(port) == -1) {
        stop();
        return;
    }
    epollLoop();
}

void TcpServer::stop() {
    listening = false;
}

int TcpServer::createEpollFd() {
    epfd = epoll_create(MAX_CONNECTION);
    if (epfd == -1) {
        printf("epoll_create failed.%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int TcpServer::bindSocket(int socket, uint16_t port) {
    sockaddr_in serv;
    bzero(&serv,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);
    if (bind(socket, (struct sockaddr*) &serv, sizeof(serv)) == -1) {
        printf("bind failed.%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int TcpServer::setUpListenSocket(uint16_t port) {
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    int hern = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &hern, sizeof(hern)) == -1) {
        printf("setsockopt failed.%s\n", strerror(errno));
        return -1;
    }
    if (listen_socket == -1) {
        printf("socket creation failed.%s\n", strerror(errno));
        return -1;
    }
    if (bindSocket(listen_socket, port) == -1) {
        printf("can't bind listen_socket socket");
        return -1;
    }
    if (listen(listen_socket, MAX_EVENTS) == -1) {
        printf("listen failed.%s\n", strerror(errno));
        return -1;
    }
    if (addSocket(listen_socket) == -1) {
        printf("adding listen_socket to epfd failed.\n");
        return -1;
    }
    return 0;
}

int TcpServer::addSocket(int socket) {
    epoll_event event;
    bzero(&event, sizeof(event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = socket;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket, &event) == -1) {
        printf("epoll_ctl failed. %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int TcpServer::makeNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        printf("can't get flags. %s\n", strerror(errno));
        return -1;
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        printf("can't set flags. %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


int TcpServer::acceptNewConnection() {
    sockaddr_in connection;
    int addrlen = sizeof(connection);
    int connection_socket = accept(listen_socket, (sockaddr*) &connection, (socklen_t*) &addrlen);
    if (connection_socket == -1) {
        printf("accept failed.%s\n", strerror(errno));
        return -1;
    }  else if (makeNonBlocking(connection_socket) == -1) {
        printf("can't make new connection non blocking");
        return -1;
    } else if (addSocket(connection_socket) == -1) {
        printf("adding new connection to epfd failed.\n");
        return -1;
    }
    return 0;
}

void TcpServer::epollLoop() {
    epoll_event events[MAX_EVENTS];
    int accepted = 0;
    while (listening) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, EPOLL_RUN_TIMEOUT);
        if (nfds < 0) {
            if (errno != EINTR) {
                printf("epoll_wait error %s\n", strerror(errno));
                stop();
                return;
            }
        }
        //std::cout << accepted << std::endl;
        for (int i = 0; i < nfds && listening; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                if (events[i].data.fd == listen_socket) {
                    printf("error in tcpsocket");
                    return;
                }
                if (epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1) {
                    printf("epoll_ctl error");
                    return;
                }

                close(events[i].data.fd);
                accepted--;
                stop();
            } else if (events[i].data.fd == listen_socket) {
                if (acceptNewConnection() == -1) {
                    printf("can't accept new connection");
                    stop();
                }
                accepted++;
            } else {
                TcpSocket socket(events[i].data.fd);
                 TcpServer::newData(socket);
             }
        }
    }
}

