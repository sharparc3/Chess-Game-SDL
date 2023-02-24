#ifndef GAMERULE_H
#define GAMERULE_H

#include "board.h"
#include <cmath>
#include <cctype>
#include <iostream>

extern bool whiteCastling;
extern bool blackCastling;

// Get piece color
int getSide(char piece);

// Get king pos
int getKingPos(char *board, int side);

// Check if square is occupied or not
bool isOccupied(char *board, int square);

// Check if piece's move capture same color piece
bool captureSameSide(char *board, int src, int dst);

// Check if piece's move is valid or not
bool isValidPieceMove(char *board, int src, int dst);

// Check if player move is valid
bool isValidPlayerMove(char* board, int src, int dst);

bool checkKingMove(char *board, int src, int dst);
bool checkQueenMove(char *board, int src, int dst);
bool checkBishopMove(char *board, int src, int dst);
bool checkPawnMove(char *board, int src, int dst);
bool checkKnightMove(char *board, int src, int dst);
bool checkRookMove(char *board, int src, int dst);

// Check if white king is in check or not
bool isWhiteKingInCheck(char* board);

// Check if black king is in check or not
bool isBlackKingInCheck(char *board);

// Check for checkmate (white)
bool isWhiteMate(char *board);

// Check for checkmate (black)
bool isBlackMate(char *board);

bool applyPromotion(char* board);

#endif