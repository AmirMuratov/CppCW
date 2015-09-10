#ifndef GAME_H
#define GAME_H

#include <QString>
const int FIELD_SIZE = 16;

class Game
{
public:
    int field[FIELD_SIZE][FIELD_SIZE];
    QString firstPlayer;
    QString secondPlayer;
    int move; //1 - first player's move 2 - otherwise
    Game(const QString & ,const QString &);
    int makeMove(int x, int y); //returns 0 if success 1 otherwise
    int checkForWin(); //0 - no one 1 - first player won 2 - second player won 3 - draw
    int checkMove();
};

#endif // GAME_H
