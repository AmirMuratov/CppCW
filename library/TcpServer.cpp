#include "TcpServer.h"

#define QUEUE_SIZE 10

TcpServer::TcpServer() {
    std::cout << "Creating TCP server." << std::endl;
}

int TcpServer::addPort(int port, std::function<void(TcpSocket*, EventType)> newData) {
    std::cout << "adding tcp port: " << port << std::endl;
    int tcpfd = createAndBind(port);
    if (tcpfd == -1) {
        printf("can't create or bind socket\n");
        return -1;
    }
    if (makeSocketNonBlocking(tcpfd) == -1) {
        printf("can't make socket nonblocking\n");
        return -1;
    }
    if (listen(tcpfd, QUEUE_SIZE) == -1) {
        perror ("listen error\n");
        return -1;
    }
    std::cout << "Adding port " << port << " to epoll" << std::endl;
    std::function<void(__uint32_t)> handlerFunction = bind(&TcpServer::connectionHandler, this, newData, tcpfd, std::placeholders::_1);
    epoll.add(tcpfd, handlerFunction, EPOLLIN | EPOLLET);
    std::cout << "Added port " << port << " to epoll" << std::endl;
    return 0;
}


void TcpServer::connectionHandler(std::function<void(TcpSocket*, EventType)> newData, int fd, __uint32_t event) {
    if (event & EPOLLRDHUP) {
        close(fd);
        return;
    }
    if (event & EPOLLERR) {
        printf("error on TCPsocket\n");
        return;
    }
    if (event & EPOLLIN) {
        std::cout << "new connection" << std::endl;
        while (1) {
            struct sockaddr in_addr;
            socklen_t in_len;
            int infd;

            in_len = sizeof in_addr;
            infd = accept(fd, &in_addr, &in_len);
            if (infd == -1) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    // We have processed all incoming connections.
                    break;
                } else {
                    printf("accept error\n");
                    break;
                }
            }

            // Make the incoming socket non-blocking and add it to the epoll.
            if (makeSocketNonBlocking(infd) == -1) {
                printf("can't make socket nonblocking");
                close(infd);
                continue;
            }
            std::function<void(__uint32_t)> handlerFunction = bind(&TcpServer::dataHandler, this, newData, infd, std::placeholders::_1);
            sockets[infd] = new TcpSocket(this, infd);
            epoll.add(infd, handlerFunction, EPOLLIN | EPOLLET | EPOLLRDHUP);
        }
    }
}

void TcpServer::dataHandler(std::function<void(TcpSocket*, EventType)> dataHandler, int fd, __uint32_t event) {
    if (event & EPOLLRDHUP) {
        dataHandler(sockets[fd], HUP);
        delete sockets[fd];
        epoll.remove(fd);
        sockets.remove(fd);
        return;
    }
    if (event & EPOLLERR) {
        dataHandler(sockets[fd], ERROR);
        delete sockets[fd];
        epoll.remove(fd);
        sockets.remove(fd);
        return;
    }
    if (event & EPOLLIN) {
        dataHandler(sockets[fd], NEWDATA);
    }
    if (event & EPOLLOUT) {
        sockets[fd]->flush();
    }
}



void TcpServer::start() {
    epoll.startListening();
}

void TcpServer::stop() {
    epoll.stopListening();
}

int TcpServer::createAndBind(int port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;
    int sfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string portstr = std::to_string(port);
    s = getaddrinfo (NULL, portstr.c_str(), &hints, &result);
    if (s != 0) {
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        int t = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) == -1) {
            printf("can't make socket reusable\n");
            continue;
        }
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            // We managed to bind successfully!
            break;
        }
        close(sfd);
    }
    if (rp == NULL) {
        // No address succeeded
        return -1;
    }
    freeaddrinfo(result);

    return sfd;
}

int TcpServer::makeSocketNonBlocking(int fd) {
    int flags;
    flags = fcntl (fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl (fd, F_SETFL, flags) == -1) {
        return -1;
    }
    return 0;
}
