#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "library/HttpResponse.h"
#include "library/HttpRequest.h"
#include "library/HttpSocket.h"
#include "library/HttpServer.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QMap>
#include <set>
#include <algorithm>

class TicTacToe {
private:
    HttpServer httpServer;
    std::vector<Game> games;
    QMap<QString, int> activePlayers;
    QMap<QString, QString> waitingPlayers;
    void gamepageRequest(HttpRequest&, HttpSocket&);
    void makeMove(HttpRequest&, HttpSocket&);
    void sendField(HttpRequest&, HttpSocket&);
    void request(HttpRequest&, HttpSocket&);

public:
    TicTacToe();
    int addPort(int);
    void start();
    void stop();
};
void request(HttpRequest& http_request, HttpSocket& socket);

#endif // TICTACTOE_H
