#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include "common.hpp"
#include <vector>   // vector
#include <stdlib.h> // rand
using namespace std;

class Board {

private:
    bitset<64> black;
    bitset<64> taken;

    bool occupied(int x, int y);
    bool get(Side side, int x, int y);
    void set(Side side, int x, int y);
    bool onBoard(int x, int y);

public:
    Board();
    ~Board();
    Board *copy();

    bool isDone();
    bool hasMoves(Side side);
    void allMoves(std::vector<Move> *moves, Side side);
    Move *randomMove(Side side);
    Move *heuristicMove(Side side);
    Move *minimax(Side side, Side home, int n);
    bool checkMove(Move *m, Side side);
    void doMove(Move *m, Side side);
    bool isCorner(Move *m);
    bool isNextToCorner(Move *m);
    int countAdvantage(Side side);
    int count(Side side);
    int countBlack();
    int countWhite();

    void setBoard(char data[]);
};

#endif
