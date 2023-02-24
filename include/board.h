#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include "texture.h"

const int SQUARE_SIZE = 80;
const int PIECE_SIZE = 70;
const int PIECE_OFFSET = 5;

const char whiteStartBoard[64] = {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

// Absolute position
const char blackStartBoard[64] = 
{
    'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    'r', 'n', 'b', 'k', 'q', 'b', 'n', 'r'
};

enum ChessPieces
{
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_ROOK,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_PAWN
};

enum Side
{
    WHITE,
    BLACK
};

int getSquareFromCoords(int mouseX, int mouseY);
void boardCopy(const char *boardSrc, char *boardDst);
void movePiece(char *board, int src, int dst);
void boardInvert(char *board);
// void renderEmptyBoard();
// void renderBoardPiece(char *board);

#endif
