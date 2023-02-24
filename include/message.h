#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

enum MsgType
{
    CHESS_LOGIN,
    CHESS_LOGIN_RESPONSE,
    CHESS_WAIT_FOR_PLAYER,
    CHESS_MATCHED,
    CHESS_OPPONENT_NAME,
    CHESS_SET_COLOR,
    CHESS_TURN,
    CHESS_WAIT_FOR_TURN,
    CHESS_BOARD_UPDATE,
    CHESS_MOVE,
    CHESS_MOVE_VALID,
    CHESS_WINNER
};

typedef struct message
{
    // Message type
    int type;
    
    bool success;

    // For sending move message
    int source;
    int dest;

    // Message content 
    char primary_message[100];
    char secondary_message[100];
} Message_t;


#endif