#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
using namespace std;

class Player {
    Side home;
    Side guest;
    Board board;
public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    void setUpBoard(char data[]);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
