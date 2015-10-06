#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "EpollWrap.h"
class TcpServer;
#include "TcpSocket.h"

enum EventType {NEWDATA, ERROR, HUP};
typedef std::shared_ptr<TcpSocket> sockptr;

class TcpServer {
    static const int QUEUE_SIZE = 10;
    int tcpfd;
    QMap<int, sockptr> sockets;
    EpollWrap epoll;
    std::vector<int> tcpsockets;
    static int makeSocketNonBlocking(int);
    static int createAndBind(int);//gets port, returns fd
    void connectionHandler(std::function<void(TcpSocket*, EventType)>, int, __uint32_t);
    void dataHandler(std::function<void(TcpSocket*, EventType)>, int, __uint32_t);

public:
    TcpServer();
    ~TcpServer();
    int addPort(int, std::function<void(TcpSocket*, EventType)>); //port, new data callback
    void start();
    void stop();
friend class TcpSocket;
};


#endif // TCPSERVER_H
