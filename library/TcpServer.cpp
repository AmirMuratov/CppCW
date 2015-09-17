#include "TcpServer.h"

#define QUEUE_SIZE 10
#define MAX_EVENTS 50
#define BUFFER_SIZE 10


TcpServer::TcpServer() {
    std::cout << "Creating TCP server." << std::endl;
}

int TcpServer::addPort(int port, std::function<void(int)> newData) {
    std::cout << "added tcp port:" << port << std::endl;
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
    std::function<void(int, __uint32_t)> handlerFunction = bind(TcpServer::connectionHandler, &epoll, newData, std::placeholders::_1, std::placeholders::_2);
    std::cout << "Process" << std::endl;
    epoll.add(tcpfd, handlerFunction, EPOLLIN | EPOLLET);
    std::cout << "Added port " << port << " to epoll" << std::endl;
    return 0;
}

void TcpServer::connectionHandler(EpollWrap *epoll, std::function<void(int)> newData, int fd, __uint32_t event) {
    if (event & EPOLLERR) {
        printf("error on TCPsocket\n");
        //
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

            /*
            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
            int s = getnameinfo(&in_addr, in_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
            if (s == 0) {
                printf("Accepted connection on descriptor %d "
                "(host=%s, port=%s)\n", infd, hbuf, sbuf);
            }
            */

            // Make the incoming socket non-blocking and add it to the epoll.
            if (makeSocketNonBlocking(infd) == -1) {
                printf("can't make socket nonblocking");
                close(infd);
                continue;
            }
            std::function<void(int, __uint32_t)> handlerFunction = bind(TcpServer::dataHandler, newData, std::placeholders::_1, std::placeholders::_2);
            epoll->add(infd, handlerFunction, EPOLLIN | EPOLLET);
        }
    }
}

void TcpServer::dataHandler(std::function<void(int)> dataHandler, int fd, __uint32_t event) {
    if (event | EPOLLERR) {
        //
    }
    if (event | EPOLLIN) {
        dataHandler(fd);
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
