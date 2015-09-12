#include "TcpServer.h"
#include <sys/eventfd.h>
#include "fcntl.h"



#define MAX_CONNECTIONS 50
#define QUEUE_SIZE 10
#define MAX_EVENTS 50
#define BUFFER_SIZE 10

//#define sptr(T) std::shared_ptr<T>

TcpServer::TcpServer(std::function<void(const TcpSocket&)> callBack) {
    dataAvailable = callBack;
    running = false;
}


static int createAndBind (int port) {
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
        close (sfd);
    }

    if (rp == NULL) {
        // No address succeeded
        return -1;
    }
    freeaddrinfo (result);

    return sfd;
}

static int makeSocketNonBlocking(int fd) {
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

void TcpServer::start(int port) {
    int tcpfd;
    int epoll_fd;
    struct epoll_event event;

    tcpfd = createAndBind(port);
    if (tcpfd == -1) {
        printf("can't create or bind socket\n");
        return;
    }
    if (makeSocketNonBlocking(tcpfd) == -1) {
        printf("can't make socket nonblocking\n");
        return;
    }

    if (listen(tcpfd, QUEUE_SIZE) == -1) {
        perror ("listen error\n");
        return;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        printf("cant create epoll instance\n");
        return;
    }

    //add tcpfd to epoll
    memset(&event, 0, sizeof(event));
    event.data.fd = tcpfd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcpfd, &event) == -1) {
        printf("epoll_ctl error\n");
        return;
    }

    struct epoll_event events[MAX_EVENTS];

    running = true;
    while (1) {
        int n;
        n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (!running) {
            break;
        }
        for (int i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                if (events[i].data.fd == tcpfd) {
                    printf("error in Tcpsocket\n");
                    return;
                }
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1) {
                    printf("epoll_ctl error\n");
                    return;
                }
            } else if (events[i].data.fd == tcpfd) {
                //new connection
                while (1) {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;

                    in_len = sizeof in_addr;
                    infd = accept(tcpfd, &in_addr, &in_len);
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
                        return;
                    }
                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, infd, &event) == -1) {
                        printf("epoll_ctl error");
                        return;
                    }
                }
            } else{
                //have some data to read
                dataAvailable(TcpSocket(events[i].data.fd));
            }
        }
    }
}

void TcpServer::stop() {
    running = false;
}
