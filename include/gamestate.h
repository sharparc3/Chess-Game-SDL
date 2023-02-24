#ifndef GAMESTATE_H
#define GAMESTATE_H

enum GameState {
    UNDEFINED,
    MATCHING, //waiting for other player
    MATCHED, // opponent found
    MATCH_INIT, // init board, piece, etc.
    MOVE, // make move and send to server
    MOVE_VALID, // server response valid move, update board
    BOARD_UPDATE,
    WAITING_FOR_TURN, // wait for turn
    GAME_OVER,
    EXIT
};

#endif