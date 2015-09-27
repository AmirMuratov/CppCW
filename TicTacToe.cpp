    #include "TicTacToe.h"
#include "Game.h"

TicTacToe::TicTacToe() {
}

int TicTacToe::addPort(int port) {
    if (httpServer.addPort(port, bind(&TicTacToe::request, this, std::placeholders::_1, std::placeholders::_2)) != 0) {
        std::cout << "Port " << port << " can't be added." << std::endl;
        return -1;
    } else {
        std::cout << "Port " << port << " added." << std::endl;
        return 0;
    }
}

void TicTacToe::start() {
    std::cout << "Starting server." << std::endl;
    httpServer.start();
}

void TicTacToe::stop() {
    httpServer.stop();
}

QString readFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("can't open %s/n", filename.toStdString().data());
        return NULL;
    }
    QByteArray total;
    QByteArray line;
    while (!file.atEnd()) {
        line = file.read(1024);
        total.append(line);
    }
    return QString(total);
}

QByteArray readBinFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return NULL;
    QByteArray total;
    QByteArray line;
    while (!file.atEnd()) {
        line = file.read(1024);
        total.append(line);
    }
    return total;
}


QString makeField(Game * g) {
    int win = g->checkForWin();
    if (win != 0) {
       if (win == 1) {
           return "<h1>" + g->firstPlayer + " win!</h1>";
       } else {
           return "<h1>" + g->secondPlayer + " win!</h1>";
       }
    }
    QString result = g->firstPlayer + "  VS  " + g->secondPlayer + "<br>";
    result.append((g->checkMove() == 1 ? g->firstPlayer : g->secondPlayer) + " turn<br>");
    result.append("<table id=\"t01\" >");
    for (int i = 0; i < FIELD_SIZE; i++) {
        result.append("<tr>");
        for (int j = 0; j < FIELD_SIZE; j++) {
            result.append("<td>");
            if (g->field[i][j] == 0) {
                result.append("&nbsp;");
            } else if (g->field[i][j] == 1) {
                result.append("X");
            } else result.append("O");
            result.append("</td>");
        }
        result.append("</tr>");
    }
    result.append("</table>");
    return result;
}

QStringList* getCookie(HttpRequest& request) {
    for (size_t i = 0; i < request.headers.size(); i++) {
        if (request.headers[i].at(0) == "Cookie:") {
            return &request.headers[i];
        }
    }
    return nullptr;
}


QString getName(HttpRequest& request) {
    QStringList* cookies = getCookie(request);
    if (cookies == nullptr) {
        return nullptr;
    }
    if (!cookies->at(1).startsWith("name=")) {
        return nullptr;
    }
    if (cookies->at(1).length() == 5) {
        return "";
    }
    return cookies->at(1).mid(5);
}

QString getPass(HttpRequest& request) {
    QStringList* cookies = getCookie(request);
    if (cookies == nullptr) {
        return nullptr;
    }
    if (!cookies->at(2).startsWith("pass=")) {
        return nullptr;
    }
    if (cookies->at(2).length() == 5) {
        return "";
    }
    return cookies->at(2).mid(5);
}

void sendIcon(HttpSocket& socket) {
    QByteArray icon(readBinFile(":/html/favicon.ico"));
    socket.write(HttpResponse(200, "TicTacToe", "image/x-icon", icon.size(), icon.data()));
}

void sendJquery(HttpSocket& socket) {
    QString jquery(readFile(":/html/jquery.js"));
    socket.write(HttpResponse(200, "TicTacToe", "text/javascript", jquery.toStdString().size(), jquery.toStdString().data()));
}

void sendMainpage(HttpSocket& socket) {
    QString mainpage(readFile(":/html/MainPage"));
    socket.write(HttpResponse(200, "TicTacToe", "text/html", mainpage.toStdString().size(), mainpage.toStdString().data()));
}

void sendGamepage(HttpSocket& socket) {
    QString gamepage(readFile(":/html/GamePage"));
    socket.write(HttpResponse(200, "TicTacToe", "text/html", gamepage.toStdString().size(), gamepage.toStdString().data()));
}

void sendWaitpage(HttpSocket& socket) {
    QString waitPage(readFile(":/html/WaitPage"));
    socket.write(HttpResponse(200, "TicTacToe", "text/html", waitPage.toStdString().size(), waitPage.toStdString().data()));
}

void sendText(HttpSocket& socket, const std::string& text) {
    socket.write(HttpResponse(200, "TicTacToe", "text/plain", text.size(), text.data()));
}

void TicTacToe::gamepageRequest(HttpRequest& http_request, HttpSocket& socket) {
    QString name = getName(http_request);
    QString pass = getPass(http_request);
    if (name == nullptr || pass == nullptr) {
        sendMainpage(socket);
    }
    if (activePlayers.count(name) != 0) {
        //User is playing game
        sendGamepage(socket);
    } else {
        //User is waiting for opponent
        if (waitingPlayers.count(pass) == 0) {
            //no opponent for user and user sends request first time
            waitingPlayers.insert(pass, name);
            sendWaitpage(socket);
        } else if (waitingPlayers.value(pass) != name) {
            //there is opponent for user
            QString player = QString(waitingPlayers.value(pass));
            games.push_back(Game(player, name));
            int size = games.size() - 1;
            activePlayers[player] = size;
            activePlayers[name] = size;
            waitingPlayers.erase(waitingPlayers.find(pass));
            sendGamepage(socket);
        } else {
            //no opponent for user
            sendWaitpage(socket);
        }
    }
}

void TicTacToe::makeMove(HttpRequest& http_request, HttpSocket& socket) {
    QString name = getName(http_request);
    if (name == nullptr) {
        sendText(socket, "ERROR");
    }
    QString data = http_request.data;
    int y = data.mid(2, data.indexOf("&") - 2).toInt();
    int x = data.mid(data.indexOf("&") + 3).toInt();
    printf("%d + %d", x, y);
    if (activePlayers.count(name) != 0) {
        int num = activePlayers.value(name);
        games[num];
        int player = games[num].firstPlayer == name ? 1 : 2;
        if (games[num].checkForWin() == 0 && player == games[num].checkMove()) {
            games[num].makeMove(x, y);
        }
        sendText(socket, "OK");
    } else {
        sendText(socket, "ERROR");
    }

}

void TicTacToe::sendField(HttpRequest& http_request, HttpSocket& socket) {
    QString name = getName(http_request);
    if (name == nullptr || activePlayers.count(name) == 0) {
        sendText(socket, "ERROR");
        return;
    }
    int num = activePlayers.value(name);
    QString response = makeField(&games[num]);
    sendText(socket, response.toStdString());
}

void TicTacToe::request(HttpRequest& http_request, HttpSocket& socket) {
    std::cout << "start processing" << std::endl;
    if (http_request.isValid() == 0) {
        socket.write(HttpResponse(400, "TicTacToe", "text/html", 0, ""));
    } else {
        std::cout << http_request.uri.toStdString() << std::endl;
        if (http_request.uri == "/favicon.ico") sendIcon(socket);
        if (http_request.uri == "/jquery.js") sendJquery(socket);
        if (http_request.uri == "/") sendMainpage(socket);
        if (http_request.uri == "/gamepage") gamepageRequest(http_request, socket);
        if (http_request.uri == "/makemove") makeMove(http_request, socket);
        if (http_request.uri.mid(0, 6) == "/field") sendField(http_request, socket);
    }
    std::cout << "finish processing" << std::endl;
}


