#include "TicTacToe.h"
#include "Game.h"

TicTacToe::TicTacToe(): httpServer(request) {}

void TicTacToe::start(uint16_t port) {
    printf("Starting TicTacToe on port %i\n", port);
    httpServer.start(port);
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
                result.append(" ");
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

QString getCookie(HttpRequest& request) {
    QString cookies = "";
    for (size_t i = 0; i < request.headers.size(); i++) {
        if (request.headers[i].size() > 1 && request.headers[i].at(0) == "Cookie:") {
            cookies.append(request.headers[i].at(1));
            for (int j = 2; j < request.headers[i].size(); j++) {
                cookies.append(" " + request.headers[i].at(j));
            }
        }
    }
    return cookies == ""? nullptr : cookies;
}


QString getName(HttpRequest& request) {
    QString cookies = getCookie(request);
    if (cookies == nullptr) {
        return nullptr;
    }
    int p = cookies.indexOf("pass=", 0);
    if (cookies.mid(0, 5) != "name=" || p == -1) {
        return nullptr;
    }
    return cookies.mid(5, p - 6);
}

QString getPass(HttpRequest& request) {
    QString cookies = getCookie(request);
    if (cookies == nullptr) {
        return nullptr;
    }
    int p = cookies.indexOf("pass=", 0);
    if (p == -1) {
        return nullptr;
    }
    return cookies.mid(p + 5);
}

std::deque<QString> messages;
std::vector<Game> games;
QMap<QString, int> activePlayers;
QMap<QString, QString> waitingPlayers;

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

void gamepageRequest(HttpRequest& http_request, HttpSocket& socket) {
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

void makeMove(HttpRequest& http_request, HttpSocket& socket) {
    QString name = getName(http_request);
    if (name == nullptr) {
        sendText(socket, "ERROR");
    }
    QString data = http_request.data;
    int y = data.mid(2, data.indexOf("&") - 2).toInt();
    int x = data.mid(data.indexOf("&") + 3).toInt();
    printf("%d + %d", x, y);
    int num = activePlayers.value(name);
    games[num];
    int player = games[num].firstPlayer == name ? 1 : 2;
    if (games[num].checkForWin() == 0 && player == games[num].checkMove()) {
        games[num].makeMove(x, y);
    }
    sendText(socket, "OK");
}

void sendField(HttpRequest& http_request, HttpSocket& socket) {
    QString name = getName(http_request);
    if (name == nullptr) {
        sendText(socket, "ERROR");
    }
    int num = activePlayers.value(name);
    QString response = makeField(&games[num]);
    sendText(socket, response.toStdString());
}

void request(HttpRequest& http_request, HttpSocket& socket) {
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
    socket.close();
}


