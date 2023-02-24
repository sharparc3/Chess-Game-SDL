#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <atomic>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/message.h"
#include "../include/board.h"
#include "../include/texture.h"
#include "../include/gamerule.h"
#include "../include/gamestate.h"
#include "../include/login.h"

// Screen dimension constants
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 640;

// Chess piece
// Define the starting positions for each piece in sprite sheet
const int pieceWidth = 135;
const int pieceHeight = 135;
const int pieceX = 0;
const int whitePiecesY = 0;
const int blackPiecesY = 135;

// Game state
// static std::atomic<GameState> currentState = UNDEFINED;
static std::atomic<GameState> currentState = MATCHING;
// GameState currentState;
std::atomic<bool> quit;
std::atomic<bool> taskDone = true;
std::atomic<bool> quitThread = false;

// Side: black or white
int playerSide = WHITE;
bool blackKingMoved, whiteKingMoved;
char board[64];
bool win;

// Renderer
SDL_Renderer *gRenderer;

// The window we'll be rendering to
SDL_Window *gWindow = nullptr;

// Font
TTF_Font *gFont;

// Scene sprites
SDL_Rect gPiecesSpriteClips[16];
LTexture gSpriteSheetTexture, lightBoardTexture, darkBoardTexture, selectTexture, checkTexture;
LTexture waitingBackground, rightWidget;

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL Error: " + std::string(SDL_GetError()) + "\n";
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            std::cout << "Warning: Linear texture filtering not enabled!\n";
        }

        // Create window
        gWindow = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr)
        {
            std::cout << "Window could not be created! SDL Error: " + std::string(SDL_GetError()) + "\n";
            success = false;
        }
        else
        {
            // Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr)
            {
                std::cout << "Renderer could not be created! SDL Error: " + std::string(SDL_GetError()) + "\n";
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    std::cout << "SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()) + "\n";
                    success = false;
                }

                // Initialize font loading
                if (TTF_Init() < 0)
                {
                    std::cout << "SDL_ttf could not be initialize";
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    gFont = TTF_OpenFont("../asset/MontserratBlack-3zOvZ.ttf", 45);
    if (gFont == nullptr)
    {
        std::cout << "Failed to load font! SDL_Error: " << SDL_GetError() << "\n";
        success = false;
    }

    // Load sprite sheet texture
    if (!gSpriteSheetTexture.loadFromFile("../asset/Chess_Pieces_Sprite.png") || !lightBoardTexture.loadFromFile("../asset/square_brown_light.png") || !darkBoardTexture.loadFromFile("../asset/square_brown_dark.png") || !selectTexture.loadFromFile("../asset/select.png") || !checkTexture.loadFromFile("../asset/check.png") || !waitingBackground.loadFromFile("../asset/waiting.png") || !rightWidget.loadFromFile("../asset/right.png"))
    {
        std::cout << "Failed to load sprite sheet texture!\n";
        success = false;
    }
    else
    {
        // Populate the gSpriteClips array with the positions of each piece
        for (int i = 0; i < 16; i++)
        {
            int x = pieceX + (i % 6) * pieceWidth;
            int y = (i < 6) ? whitePiecesY : blackPiecesY;
            gPiecesSpriteClips[i] = {x, y, pieceWidth, pieceHeight};
        }
    }

    return success;
}

void cleanUp()
{
    // Free loaded images
    gSpriteSheetTexture.free();
    gSpriteSheetTexture.free();
    lightBoardTexture.free();
    darkBoardTexture.free();
    selectTexture.free();
    checkTexture.free();
    waitingBackground.free();
    rightWidget.free();

    // Destroy window
    TTF_CloseFont(gFont);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool loadFont()
{
    // Initialize font loading
    if (TTF_Init() < 0)
    {
        std::cout << "SDL_ttf could not be initialize";
        return false;
    }
    gFont = TTF_OpenFont("../asset/MontserratBlack-3zOvZ.ttf", 45);
    if (gFont == nullptr)
    {
        std::cout << "Failed to load font! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }
    return true;
}

SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == nullptr)
    {
        SDL_Log("Failed to render text: %s", TTF_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

void renderEmptyBoard()
{
    // Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    // Loop through each square on the board
    LTexture *textureToRender;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // Calculate the position to render the square
            int x = j * SQUARE_SIZE;
            int y = i * SQUARE_SIZE;

            // Determine which texture to use based on the square's position
            textureToRender = (i + j) % 2 == 0 ? &lightBoardTexture : &darkBoardTexture;

            // Render the square
            textureToRender->render(x, y, SQUARE_SIZE, SQUARE_SIZE);
        }
    }
}

void renderBoardPiece(char *board)
{
    int xPos, yPos;
    SDL_Rect piece;
    for (int i = 0; i < 64; i++)
    {
        xPos = PIECE_OFFSET + (i % 8) * SQUARE_SIZE;
        yPos = PIECE_OFFSET + i / 8 * SQUARE_SIZE;

        switch (board[i])
        {
        case ' ':
            continue;

        case 'K':
            piece = gPiecesSpriteClips[WHITE_KING];
            break;
        case 'Q':
            piece = gPiecesSpriteClips[WHITE_QUEEN];
            break;
        case 'B':
            piece = gPiecesSpriteClips[WHITE_BISHOP];
            break;
        case 'N':
            piece = gPiecesSpriteClips[WHITE_KNIGHT];
            break;
        case 'R':
            piece = gPiecesSpriteClips[WHITE_ROOK];
            break;
        case 'P':
            piece = gPiecesSpriteClips[WHITE_PAWN];
            break;
        case 'k':
            piece = gPiecesSpriteClips[BLACK_KING];
            break;
        case 'q':
            piece = gPiecesSpriteClips[BLACK_QUEEN];
            break;
        case 'b':
            piece = gPiecesSpriteClips[BLACK_BISHOP];
            break;
        case 'n':
            piece = gPiecesSpriteClips[BLACK_KNIGHT];
            break;
        case 'r':
            piece = gPiecesSpriteClips[BLACK_ROOK];
            break;
        case 'p':
            piece = gPiecesSpriteClips[BLACK_PAWN];
            break;

        default:
            break;
        }

        // Render piece
        gSpriteSheetTexture.render(xPos, yPos, PIECE_SIZE, PIECE_SIZE, &piece);
    }
}

void messageHanler(int client_fd, Message_t *response)
{
    std::cout << "-->Thread created\n";
    int bytes;
    while (!quitThread)
    {
        if (taskDone)
        {
            // messageMutex.lock();
            memset(response, 0, sizeof(Message_t));
            bytes = recv(client_fd, response, sizeof(Message_t), 0);
            if (bytes <= 0)
            {
                std::cout << "Server disconnected\n";
                quitThread = true;
                quit = true;
                // messageMutex.unlock();
                return;
            }
            switch (response->type)
            {
            case CHESS_SET_COLOR:
                playerSide = response->source;
                std::cout << "Received SET_COLOR: " << response->source << "\n";
                currentState = MATCH_INIT;
                break;
            case CHESS_WAIT_FOR_TURN:
                currentState = WAITING_FOR_TURN;
                std::cout << "Received WAITING FOR TURN\n";
                break;
            case CHESS_TURN:
                currentState = MOVE;
                std::cout << "Received TURN\n";
                break;
            case CHESS_MOVE_VALID:
                currentState = MOVE_VALID;
                std::cout << "Received MOVE_VALID\n";
                std::cout << "Received move from server: " << response->source << ":" << response->dest << "\n";
                break;
            case CHESS_BOARD_UPDATE:
                currentState = BOARD_UPDATE;
                std::cout << "Received BOARD_UPDATE\n";
                // boardCopy(response->primary_message, board);
                break;
            case CHESS_WINNER:
                currentState = GAME_OVER;
                // win = response->success == true ? true : false;
                win = response->success;
                std::cout << "Received WINNER\n";
                quitThread = true;
                break;

            default:
                std::cout << "Invalid message format\n";
                break;
            }
            taskDone = false;
        }
        // messageMutex.unlock();
        SDL_Delay(100);
    }
    std::cout << "-->Thread terminated\n";
}

int main(int argc, char **argv)
{
    LoginData loginData;
    Message_t *response = new Message_t;
    Message_t *request = new Message_t;
    // currentState = MATCHING;
    quit = false;
    SDL_Event e;
    SDL_Texture *textTexture;
    SDL_Color textColor;
    SDL_Rect textRect;
    char whiteBoard[64];
    bool overrideRender = false;

    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // store socket info
    loginData.server_address = server_address;

    if (!loginData.loggedIn)
    {
        gtk_init(&argc, &argv);
        loginScreen(&loginData);
        std::cout << "Hello, " << loginData.user << std::endl;
    }
    if (!loadFont())
    {
        quit = true;
    }

    // Initialize SDL
    if (!init())
    {
        std::cout << "Failed to initialize!\n";
        quit = true;
    }

    // load media into texture
    else if (!loadMedia())
    {
        std::cout << "Failed to load media!\n";
        quit = true;
    }

    int mouseX, mouseY, clickedSquare = -1, square, checkPos;
    int selectedX = -1, selectedY = -1;

    // create thread for message receiving
    std::thread responseHandler(messageHanler, *loginData.client_fd, response);
    responseHandler.detach();

    // main game loop
    while (!quit && loginData.loggedIn)
    {
        // Handle events
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                quitThread = true;
            }
            switch (currentState)
            {
            case MATCHING:
                // std::cout << "State: MATCHING\n";
                // Render loading screen
                if (taskDone)
                {
                    std::cout << "Rendering loading screen\n";
                    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(gRenderer);
                    waitingBackground.render(0, 0, 640, 640);
                    textColor = {0x29, 0x29, 0x29};
                    textTexture = renderText(gRenderer, gFont, "Waiting for other player", textColor);
                    textRect = {70, 523, 497, 54};
                    SDL_RenderCopy(gRenderer, textTexture, nullptr, &textRect);
                    overrideRender = true;
                    SDL_Delay(500);
                }
                break;

            case MATCH_INIT:
                if (taskDone)
                    break;
                else
                {
                    // std::cout << "State: MATCH_INIT\n";
                    boardCopy(blackStartBoard, board);
                    checkPos = -1;
                    // std::cout << "Game initialized\n";
                    taskDone = true;
                }

                break;

            case WAITING_FOR_TURN:
                if (taskDone)
                    break;
                else
                {
                    taskDone = true;
                }
                // std::cout << "State: WAITING_FOR_TURN\n";

                break;

            case MOVE:
                // std::cout << "State: MOVE\n";
                int absoluteSquare, absoluteClickedSquare;
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    SDL_GetMouseState(&mouseX, &mouseY);
                    checkPos = -1;
                    square = getSquareFromCoords(mouseX, mouseY);
                    if (square >= 0)
                    {
                        if (clickedSquare < 0)
                        {
                            if (playerSide == WHITE)
                            {
                                absoluteSquare = 63 - square;
                            }
                            else
                            {
                                absoluteSquare = square;
                            }

                            if (board[absoluteSquare] == ' ' || getSide(board[absoluteSquare]) != playerSide)
                            {
                                // Is first click on empty square
                                clickedSquare = -1;
                            }
                            else
                            {
                                // Is first click on a piece
                                // Set first square
                                clickedSquare = square;
                            }
                        }
                        else
                        {
                            // Is second click
                            if (playerSide == WHITE)
                            {
                                absoluteClickedSquare = 63 - clickedSquare;
                                absoluteSquare = 63 - square;
                            }
                            else
                            {
                                absoluteClickedSquare = clickedSquare;
                                absoluteSquare = square;
                            }
                            std::cout << "White move: " << clickedSquare << ":" << square << "\n";
                            std::cout << "Absolute move: " << absoluteClickedSquare << ":" << absoluteSquare << "\n";
                            if (isValidPlayerMove(board, absoluteClickedSquare, absoluteSquare))
                            {
                                // if (isWhiteKingInCheck(board))
                                // {
                                //     std::cout << "White king is in check\n";
                                //     checkPos = getKingPos(board, WHITE);
                                //     if (playerSide == WHITE)
                                //     {
                                //         checkPos = 63 - checkPos;
                                //     }
                                // }
                                // if (isBlackKingInCheck(board))
                                // {
                                //     std::cout << "Black king is in check\n";
                                //     checkPos = getKingPos(board, BLACK);
                                //     if (playerSide == WHITE)
                                //     {
                                //         checkPos = 63 - checkPos;
                                //     }
                                // }

                                // messageMutex.lock();
                                memset(request, 0, sizeof(Message_t));
                                request->type = CHESS_MOVE;
                                request->source = absoluteClickedSquare;
                                request->dest = absoluteSquare;

                                send(*loginData.client_fd, request, sizeof(Message_t), 0);
                                // messageMutex.unlock();
                                // std::cout << "Absolute MOVE sent: " << response->source << ":" << response->dest << "\n";
                                // messageMutex.unlock();
                                taskDone = true;
                                // reset clickedSquare
                                clickedSquare = -1;
                            }
                            else
                            {
                                // Invalid move
                                clickedSquare = -1;
                            }
                        }
                    }
                }

                break;
            case MOVE_VALID:
                if (taskDone)
                    break;
                else
                {
                    movePiece(board, response->source, response->dest);
                    std::cout << "Piece moved: " << response->source << ":" << response->dest << "\n";
                    // currentState = WAITING_FOR_TURN;
                    taskDone = true;
                }
                // std::cout << "State: MOVE_VALID\n";
                // Move piece

                break;

            case BOARD_UPDATE:
                if (taskDone)
                    break;
                else
                {
                    // std::cout << "State: BOARD_UPDATE\n";
                    boardCopy(response->primary_message, board);
                    taskDone = true;
                }

                break;

            case GAME_OVER:
                // std::cout << "State: GAME_OVER\n";
                if (response->success)
                {
                    std::cout << "You won\n";
                }
                else
                {
                    std::cout << "You lose\n";
                }
                currentState = EXIT;
                break;

            case EXIT:
                // std::cout << "State: EXIT\n";
                if (e.type = SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        quit = true;
                    }
                }
                quit = true;
                break;

            default:
                break;
            }

            if (!overrideRender)
            {
                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Render board
                renderEmptyBoard();

                // Render board
                if (playerSide == BLACK)
                {
                    renderBoardPiece(board);
                }
                else
                {
                    boardCopy(board, whiteBoard);
                    boardInvert(whiteBoard);
                    renderBoardPiece(whiteBoard);
                }

                // Render selected piece
                // SDL_SetTextureColorMod(selectTexture.getTexture(), 255, 255, 0);
                selectedX = clickedSquare % 8 * SQUARE_SIZE;
                selectedY = clickedSquare / 8 * SQUARE_SIZE;
                selectTexture.render(selectedX, selectedY, SQUARE_SIZE, SQUARE_SIZE);

                // get check position
                if (isWhiteKingInCheck(board))
                {
                    checkPos = getKingPos(board, WHITE);
                }
                else if (isBlackKingInCheck(board))
                {
                    checkPos = getKingPos(board, BLACK);
                }
                if (playerSide == WHITE)
                    checkPos = 63 - checkPos;

                // render check warning
                if (checkPos > 0)
                {
                    checkTexture.render(checkPos % 8 * SQUARE_SIZE, checkPos / 8 * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
                }
            }
            overrideRender = false;
            rightWidget.render(640, 0, 260, 640);

            // Update screen
            SDL_RenderPresent(gRenderer);
        }
        // responseHandler.join();
        SDL_Delay(30);
    }
    // Free resources and close SDL
    cleanUp();
    delete response;
    delete request;
    return 0;
}

// compile
// g++ main.cpp ../object/*.o -o main -lSDL2 -lSDL2_image
// gtk flag: `pkg-config --cflags --libs gtk+-3.0`
// g++ client.cpp ../object/board.o ../object/gamerule.o ../object/init.o ../object/login.o ../object/texture.o -o client -lSDL2 -lSDL2_image -lSDL2_ttf `pkg-config --cflags --libs gtk+-3.0`