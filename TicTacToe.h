#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "lib/http/HttpResponse.h"
#include "lib/http/HttpRequest.h"
#include "lib/http/HttpSocket.h"
#include "lib/http/HttpServer.h"
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
    void start(uint16_t port);
    void stop();
};
void request(HttpRequest& http_request, HttpSocket& socket);

#endif // TICTACTOE_H
