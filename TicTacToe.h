#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "library/HttpResponse.h"
#include "library/HttpRequest.h"
#include "library/HttpSocket.h"
#include "library/HttpServer.h"
#include "Game.h"
#include <QObject>
#include <QFile>
#include <deque>
#include <QUrl>
#include <vector>
#include <QMap>
#include <set>
#include <algorithm>

class TicTacToe {
private:
    HttpServer httpServer;

public:
    TicTacToe();
    void addPort(int);
    void start();
    void stop();
};
void request(HttpRequest& http_request, HttpSocket& socket);

#endif // TICTACTOE_H
