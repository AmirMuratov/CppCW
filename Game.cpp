#include "Game.h"

Game::Game(const QString & first, const QString & second) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field[i][j] = 0;
        }
    }
    move = 1;
    firstPlayer = QString(first);
    secondPlayer = QString(second);
}


int Game::makeMove(int x, int y) {
    if ((x < 0) || (y < 0) || (x > FIELD_SIZE - 1) || (y > FIELD_SIZE - 1) || (field[x][y] != 0)) {
        return 1;
    }
    if (move == 1) {
        field[x][y] = 1;
        move = 2;
    } else {
        field[x][y] = 2;
        move = 1;
    }
    return 0;
}

bool check(int t, const Game * g) {
    bool win = false;
    int h;
    int v;
    for (int i = 0; i < FIELD_SIZE; i++) {
        h = 0;
        v = 0;
        for (int j = 0; j < FIELD_SIZE; j++) {
            if (g->field[i][j] == t) {
                h++;
            } else {
                h = 0;
            }
            if (g->field[j][i] == t) {
                v++;
            } else {
                v = 0;
            }
            if ((h == 5) || (v == 5)) {
                win = true;
            }
        }
    }
    int x = 0;
    int y = FIELD_SIZE - 1;
    int w;
    while (x != FIELD_SIZE - 1 || y != 0) {
        int x1 = x;
        int y1 = y;
        w = 0;
        while (x1 < FIELD_SIZE && y1 < FIELD_SIZE) {
            if (g->field[x1][y1] == t) {
                w++;
            } else {
                w = 0;
            }
            if (w == 5) {
                win = true;
            }
            x1++;
            y1++;
        }
        if (y > 0) {
            y--;
        } else {
            x++;
        }
    }

    x = 0;
    y = 0;
    while (x != FIELD_SIZE - 1 || y != FIELD_SIZE - 1) {
        int x1 = x;
        int y1 = y;
        w = 0;
        while (x1 < FIELD_SIZE && y1 >= 0) {
            if (g->field[x1][y1] == t) {
                w++;
            } else {
                w = 0;
            }
            if (w == 5) {
                win = true;
            }
            x1++;
            y1--;
        }
        if (y < FIELD_SIZE - 1) {
            y++;
        } else {
            x++;
        }
    }
    return win;
}

bool fieldIsFull(const Game * g) {
    int k = 0;
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            k += g->field[i][j] == 0 ? 1 : 0;
        }
    }
    return k == 0;
}

int Game::checkForWin() {
    if (check(1, this)) {
        return 1;
    }
    if (check(2, this)) {
        return 2;
    }
    if (fieldIsFull(this)) {
        return 3;
    }
    return 0;
}

int Game::checkMove() {
    return move;
}

