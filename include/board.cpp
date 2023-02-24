#include "board.h"

int getSquareFromCoords(int mouseX, int mouseY)
{
    return (mouseY / SQUARE_SIZE) * 8 + (mouseX / SQUARE_SIZE);
}

void boardCopy(const char *boardSrc, char *boardDst)
{
    for (int i = 0; i < 64; i++)
    {
        boardDst[i] = boardSrc[i];
    }
    boardDst[64] = '\0';
}

// void boardInvert(const char *boardSrc, char *boardDst)
// {
//     int j = 0;
//     for (int i = 63; i >= 0; i--)
//     {
//         boardDst[j] = boardSrc[i];
//         j++;
//     }
// }

void boardInvert(char *board)
{
    char tempBoard[64];
    boardCopy(board, tempBoard);
    int j = 0;
    for (int i = 63; i >= 0; i--)
    {
        board[j] = tempBoard[i];
        j++;
    }
}

void movePiece(char *board, int src, int dst)
{
    board[dst] = board[src];
    board[src] = ' ';
}

