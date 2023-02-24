#include "gamerule.h"

int getSide(char piece)
{
    if (piece >= 'A' && piece <= 'Z')
        return WHITE;
    return BLACK;
}

bool isOccupied(char *board, int square)
{
    return board[square] != ' ';
}

int getKingPos(char *board, int side)
{
    int pos;
    for (int i = 0; i < 64; i++)
    {
        if (side == BLACK && board[i] == 'k')
        {
            pos = i;
            break;
        }
        if (side == WHITE && board[i] == 'K')
        {
            pos = i;
            break;
        }
    }
    return pos;
}

bool captureSameSide(char *board, int src, int dst)
{
    if (isOccupied(board, dst) && getSide(board[src]) == getSide(board[dst]))
        return true;
    return false;
}

// bool checkKingMove(char *board, int src, int dst)
bool checkKingMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    if (board[58] == ' ' && board[57] == ' ' && board[56] == 'r' && src == 59 && dst == 57) return true;
    if (board[1] == ' ' && board[2] == ' ' && board[0] == 'R' && src == 3 && dst == 1) return true;
    // if (!blackKingMoved && board[58] == ' ' && board[57] == ' ' && board[56] == 'r' && src == 59 && dst == 57) return true;
    // if (!whiteKingMoved && board[1] == ' ' && board[2] == ' ' && board[0] == 'R' && src == 3 && dst == 1) return true;

    // Check if the destination square is within one square of the source square
    if (abs(dstX - srcX) > 1 || abs(dstY - srcY) > 1)
    {
        return false;
    }

    // Check if the destination square is not occupied by a friendly piece
    if (board[dst] != ' ' && getSide(board[dst]) == getSide(board[src]))
    {
        return false;
    }

    return true;
}

bool checkQueenMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    int dx = abs(dstX - srcX);
    int dy = abs(dstY - srcY);

    if (dx == 0 && dy == 0)
    {
        return false; // invalid move
    }

    if (dx == dy)
    {
        // diagonal move
        int xdir = (dstX > srcX) ? 1 : -1;
        int ydir = (dstY > srcY) ? 1 : -1;

        for (int i = 1; i < dx; i++)
        {
            int x = srcX + i * xdir;
            int y = srcY + i * ydir;
            // if (board[8 * y + x] != ' ')
            if (isOccupied(board, 8 * y + x))
            {
                return false; // a piece blocks the diagonal path
            }
        }
    }
    else if (dx == 0)
    {
        // vertical move
        int ydir = (dstY > srcY) ? 1 : -1;

        for (int y = srcY + ydir; y != dstY; y += ydir)
        {
            // if (board[8 * y + srcX] != ' ')
            if (isOccupied(board, 8 * y + srcX))
            {
                return false; // a piece blocks the vertical path
            }
        }
    }
    else if (dy == 0)
    {
        // horizontal move
        int xdir = (dstX > srcX) ? 1 : -1;

        for (int x = srcX + xdir; x != dstX; x += xdir)
        {
            // if (board[8 * srcY + x] != ' ')
            if (isOccupied(board, 8 * srcY + x))
            {
                return false; // a piece blocks the horizontal path
            }
        }
    }
    else
    {
        return false; // invalid move
    }

    return true;
}

bool checkBishopMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    // Make sure the move is diagonal
    if (abs(dstX - srcX) != abs(dstY - srcY))
    {
        return false;
    }

    // Check for any pieces in the bishop's path
    int dX = (dstX > srcX) ? 1 : -1;
    int dY = (dstY > srcY) ? 1 : -1;
    int x = srcX + dX;
    int y = srcY + dY;

    while (x != dstX && y != dstY)
    {
        if (board[y * 8 + x] != ' ')
        {
            return false;
        }
        x += dX;
        y += dY;
    }
    return true;
}

bool checkPawnMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    // Check if the pawn is white
    if (board[src] == 'P')
    {
        // Check if the move is a one-square move forward
        if (dstY == srcY + 1 && dstX == srcX)
        {
            // Check if the destination square is empty
            if (board[dst] == ' ')
            {
                return true;
            }
        }
        // Check if the move is a two-square move forward
        else if (dstY == srcY + 2 && dstX == srcX && srcY == 1)
        {
            // Check if the intermediate square is empty and the destination square is empty
            if (board[src + 8] == ' ' && board[dst] == ' ')
            {
                return true;
            }
        }
        // Check if the move is a diagonal capture
        else if (dstY == srcY + 1 && abs(dstX - srcX) == 1)
        {
            // Check if the destination square contains an opponent's piece
            if (board[dst] != ' ' && getSide(board[dst]) == BLACK)
            {
                return true;
            }
        }
    }
    else
        // Check if the pawn is black
        if (board[src] == 'p')
        {
            // Check if the move is a one-square move forward
            if (dstY == srcY - 1 && dstX == srcX)
            {
                // Check if the destination square is empty
                if (board[dst] == ' ')
                {
                    return true;
                }
            }
            // Check if the move is a two-square move forward
            else if (dstY == srcY - 2 && dstX == srcX && srcY == 6)
            {
                // Check if the intermediate square is empty and the destination square is empty
                if (board[src - 8] == ' ' && board[dst] == ' ')
                {
                    return true;
                }
            }
            // Check if the move is a diagonal capture
            else if (dstY == srcY - 1 && (dstX == srcX - 1 || dstX == srcX + 1))
            {
                // Check if the destination square contains an opponent's piece
                if (board[dst] != ' ' && getSide(board[dst]) == WHITE)
                {
                    return true;
                }
            }
        }
    return false;
}

bool checkKnightMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    int dx = abs(dstX - srcX);
    int dy = abs(dstY - srcY);

    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

bool checkRookMove(char *board, int src, int dst)
{
    // Calculate coordinate
    int srcX, srcY, dstX, dstY;
    srcX = src % 8;
    srcY = src / 8;
    dstX = dst % 8;
    dstY = dst / 8;

    if (srcX != dstX && srcY != dstY)
    {
        // Not moving along a straight line
        return false;
    }

    int step = 0;
    if (srcX == dstX)
    {
        // Moving vertically
        step = (srcY < dstY) ? 1 : -1;
        for (int y = srcY + step; y != dstY; y += step)
        {
            if (board[y * 8 + srcX] != ' ')
            {
                // There's a piece in the way
                return false;
            }
        }
    }
    else
    {
        // Moving horizontally
        step = (srcX < dstX) ? 1 : -1;
        for (int x = srcX + step; x != dstX; x += step)
        {
            if (board[srcY * 8 + x] != ' ')
            {
                // There's a piece in the way
                return false;
            }
        }
    }

    return true;
}

bool isValidPieceMove(char *board, int src, int dst)
{
    // return flag
    bool flag = false;

    if (src == dst || board[src] == ' ' || captureSameSide(board, src, dst))
        return flag;

    // TODO: piece movement check
    switch (board[src])
    {
    case 'k':
    case 'K':
        flag = checkKingMove(board, src, dst);
        break;
    case 'q':
    case 'Q':
        flag = checkQueenMove(board, src, dst);
        break;
    case 'b':
    case 'B':
        flag = checkBishopMove(board, src, dst);
        break;
    case 'r':
    case 'R':
        flag = checkRookMove(board, src, dst);
        break;
    case 'n':
    case 'N':
        flag = checkKnightMove(board, src, dst);
        break;
    case 'p':
    case 'P':
        flag = checkPawnMove(board, src, dst);
        break;

    default:
        break;
    }

    return flag;
}

bool isWhiteKingInCheck(char *board)
{
    int whiteKingPos;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] == 'K')
        {
            whiteKingPos = i;
            break;
        }
    }
    for (int i = 0; i < 64; i++)
    {
        switch (board[i])
        {
        case 'k':
        case 'q':
        case 'n':
        case 'b':
        case 'r':
        case 'p':
            if (isValidPieceMove(board, i, whiteKingPos))
            {
                // std::cout << "check\n";
                return true;
            }

        default:
            break;
        }
    }
    return false;
}

bool isBlackKingInCheck(char *board)
{
    int blackKingPos;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] == 'k')
        {
            blackKingPos = i;
            break;
        }
    }
    for (int i = 0; i < 64; i++)
    {

        switch (board[i])
        {
        case 'K':
        case 'Q':
        case 'N':
        case 'B':
        case 'R':
        case 'P':
            if (isValidPieceMove(board, i, blackKingPos))
            {
                // std::cout << "check\n";
                return true;
            }

        default:
            break;
        }
    }
    return false;
}

bool isWhiteMate(char *board)
{
    // If king is check
    if (!isWhiteKingInCheck(board))
    {
        return false;
    }

    bool isMate = true, stillInCheck = true;
    int kingPos;
    char tempPiece;

    // Get white king position
    for (int i = 0; i < 64; i++)
    {
        if (board[i] == 'K')
            kingPos = i;
    }

    // Find a piece
    for (int i = 0; i < 64; i++)
    {
        // If white piece then make a false move and check
        if (board[i] >= 'A' && board[i] <= 'Z')
        {
            for (int j = 0; j < 64; j++)
            {
                // Skip if same side
                if (board[j] >= 'A' && board[j] <= 'Z')
                    continue;

                // Check if the false move is valid
                if (isValidPieceMove(board, i, j))
                {
                    // Make the move and check if the king is still in check
                    tempPiece = board[j];
                    board[j] = board[i];
                    board[i] = ' ';

                    // If white king is not in check
                    if (!isWhiteKingInCheck(board))
                    {
                        stillInCheck = false;
                        // Undo the move
                        board[i] = board[j];
                        board[j] = tempPiece;
                        break;
                    }
                    else
                    {
                        // Undo the move
                        board[i] = board[j];
                        board[j] = tempPiece;
                    }
                }
            }
        }
        // If the king isn't in check anymore
        if (!stillInCheck)
        {
            isMate = false;
            break;
        }
    }

    return isMate;
}

bool isBlackMate(char *board)
{
    // If king is check
    if (!isBlackKingInCheck(board))
    {
        return false;
    }

    bool isMate = true, stillInCheck = true;
    int kingPos;
    char tempPiece;

    // Get black king position
    for (int i = 0; i < 64; i++)
    {
        if (board[i] == 'k')
            kingPos = i;
    }

    // Find a piece
    for (int i = 0; i < 64; i++)
    {
        // If black piece then make a false move and check
        if (board[i] >= 'a' && board[i] <= 'z')
        {
            for (int j = 0; j < 64; j++)
            {
                // Skip if same side
                if (board[j] >= 'a' && board[j] <= 'z')
                    continue;

                // Check if the false move is valid
                if (isValidPieceMove(board, i, j))
                {
                    // Make the move and check if the king is still in check
                    tempPiece = board[j];
                    board[j] = board[i];
                    board[i] = ' ';

                    // If white king is not in check
                    if (!isBlackKingInCheck(board))
                    {
                        stillInCheck = false;
                        // Undo the move
                        board[i] = board[j];
                        board[j] = tempPiece;
                        break;
                    }
                    else
                    {
                        // Undo the move
                        board[i] = board[j];
                        board[j] = tempPiece;
                    }
                }
            }
        }
        // If the king isn't in check anymore
        if (!stillInCheck)
        {
            isMate = false;
            break;
        }
    }

    return isMate;
}

bool applyPromotion(char *board)
{
    bool flag = false;
    for (int i = 0; i < 8; i++)
    {
        if (board[i] == 'p') 
        {
            board[i] = 'q';
            flag = true;
        }
    }
    for (int i = 63; i >= 56; i--)
    {
        if (board[i] == 'P')
        {
            board[i] = 'Q';
            flag = true;
        }
    }
    return flag;
}

bool isValidPlayerMove(char *board, int src, int dst)
{
    bool valid = false;
    int side = getSide(board[src]);
    char tempBoard[64];
    if (isValidPieceMove(board, src, dst))
    {
        boardCopy(board, tempBoard);
        tempBoard[dst] = tempBoard[src];
        tempBoard[src] = ' ';
        if (side == BLACK && isBlackKingInCheck(tempBoard))
            valid = false;
        else if (side == WHITE && isWhiteKingInCheck(tempBoard))
            valid = false;
        else
            valid = true;
    }
    if (valid && board[src] == 'K' && dst == 1 && src == 3)
    {
        board[0] = ' ';
        board[2] = 'R';
    }
    if (valid && board[src] == 'k' && dst == 57 && src == 59)
    {
        board[56] = ' ';
        board[58] = 'r';
    }
    return valid;
}