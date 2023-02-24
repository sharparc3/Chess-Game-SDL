#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <random>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../include/message.h"
#include "../include/board.h"
// #include "../include/timer.h"
#include "../include/gamerule.h"

static int client_id = 10;
constexpr int kPort = 5000;
const int MAX_PLAYER = 10;
std::atomic<bool> running{true};

typedef struct Client
{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    int color;
    bool loggedIn = false;
    bool connected = true;
    bool joined = false;
    char name[64];
} Client_t;

typedef struct GameRoom
{
    Client_t *client1;
    Client_t *client2;
    bool started = false;
} Game_t;

// vector contains connected client
std::vector<Client_t *> client_container;
// std::mutex client_mutex;

// std::vector<Game_t *> game_rooms(5);
Game_t *game_rooms = new Game_t[5];
// std::mutex room_mutex;

// user information file
std::ifstream loginFile("user.txt", std::ios::in);

int sendToClient(Client_t *client, Message_t *msg)
{
    int byte = send((*client).sockfd, msg, sizeof(Message_t), 0);
    return byte;
}

void handle_client(Client_t *client)
{
    std::cout << "New client accepted" << std::endl;
    std::string message;
    char buffer[1024] = {0};
    int bytesReceived;
    while (true)
    {
        bytesReceived = recv(client->sockfd, buffer, 1024, 0);
        if (bytesReceived < 0)
        {
            std::cout << "Receive message error\n";
            break;
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected\n";
            break;
        }
        message = std::string(buffer);
        std::cout << "Received message: " << message << std::endl;
        message = message + " - from Server.";

        // std::string welcome_message = "Welcome to the server!";
        // Send back message
        send(client->sockfd, message.c_str(), message.length() + 1, 0);
    }
    close(client->sockfd);
    client_container.erase(std::remove_if(client_container.begin(), client_container.end(),
                                          [client](const Client_t *c)
                                          { return c->uid == client->uid; }),
                           client_container.end());

    // client_container.erase;
    delete client;
}

void login_handler(Client_t *client)
{
    std::cout << "Login thread started" << std::endl;
    bool validLogin = false;
    Message_t *message, *response = new Message_t;
    char buffer[sizeof(Message_t)] = {0};
    int bytesReceived;

    bytesReceived = recv(client->sockfd, buffer, sizeof(Message_t), 0);
    message = (Message_t *)buffer;
    if (bytesReceived < 0)
    {
        std::cout << "Receive message error\n";
    }
    if (bytesReceived == 0)
    {
        std::cout << "Client disconnected\n";
    }
    if (message->type != CHESS_LOGIN)
    {
        std::cout << "Not login message\n";
    }
    else
    {
        if (!loginFile.is_open())
        {
            std::cout << "Can't open user information file!\n";
        }
        else
        {
            std::string username, password;
            std::string recvUsername = std::string(message->primary_message);
            std::string recvPassword = std::string(message->secondary_message);
            loginFile.clear();
            loginFile.seekg(0);
            // std::cout << "Received: " << recvUsername << ":" << recvPassword << std::endl;
            while (loginFile >> username >> password)
            {
                // std::cout << "Checking: " << username << ":" << password << std::endl;
                if (username == recvUsername && password == recvPassword)
                {
                    // std::cout << "Matched: " << username << ":" << password << "\n";
                    validLogin = true;
                    break;
                }
            }
        }
    }

    if (!validLogin)
    {
        std::cout << "Authentication failed\n";
        // Send decline message and clean up
        response->type = CHESS_LOGIN_RESPONSE;
        response->success = false;
        if (send(client->sockfd, response, sizeof(Message_t), 0) > 0)
        {
            std::cout << "Decline message sent\n";
        }
        std::cout << "Client connection closed\n";
        close(client->sockfd);
        delete client;
        delete response;
    }
    else
    {
        // Login successfully
        response->type = CHESS_LOGIN_RESPONSE;
        response->success = true;
        client->loggedIn = true;
        send(client->sockfd, response, sizeof(Message_t), 0);

        // Add client to container
        // client_mutex.lock();
        client_container.push_back(client);
        // client_mutex.unlock();
        delete response;
    }
    std::cout << "Login thread terminated\n\n";
}

int side_roll()
{
    // Create a random number generator and seed it with a random device
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the distribution for the random values (uniform distribution between 0 and 1)
    std::uniform_int_distribution<> dist(0, 1);

    // Generate a random integer value between 0 and 1
    return dist(gen);
}

int switchSide(int turn)
{
    int side = turn == BLACK ? WHITE : BLACK;
    return side;
}

void setZero(Message_t *message)
{
    memset(message, 0, sizeof(Message_t));
}

void game_room_handler(Game_t *room)
{
    // client_mutex.lock();
    std::cout << "Game started\n";
    int client1_color, client2_color;
    Client_t *player1 = room->client1;
    Client_t *player2 = room->client2;
    Message_t *message1 = new Message_t;
    Message_t *message2 = new Message_t;
    Message_t *response = new Message_t;

    // announce match
    // message1->type = CHESS_MATCHED;
    // message2->type = CHESS_MATCHED;
    // sendToClient(player1, message1);
    // sendToClient(player2, message2);
    // std::cout << "MATCHED message sent\n";
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // set color to client
    player1->color = side_roll();
    player2->color = switchSide(player1->color);

    // send color to client
    setZero(message1);
    setZero(message2);
    message1->type = CHESS_SET_COLOR;
    message1->source = player1->color;
    message2->type = CHESS_SET_COLOR;
    message2->source = player2->color;
    sendToClient(player1, message1);
    sendToClient(player2, message2);
    std::cout << "SET_COLOR message sent\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // game init
    char board[64];
    boardCopy(blackStartBoard, board);
    // std::cout << board << "\n";
    int byteRecv;
    int turn = WHITE;
    int checkPos, winner;
    bool endMatch = false;
    bool whiteKingMoved = false, blackKingMoved = false;
    bool checkmate = false;

    // Match started
    while (!endMatch)
    {
        // move handling
        if (player1->color == turn)
        {
            setZero(message1);
            setZero(message2);
            message1->type = CHESS_TURN;
            message2->type = CHESS_WAIT_FOR_TURN;
            sendToClient(player1, message1);
            std::cout << "TURN message sent to player1\n";
            sendToClient(player2, message2);
            std::cout << "WAIT_FOR_TURN message sent to player2\n";
            byteRecv = recv(player1->sockfd, response, sizeof(Message_t), 0);
            std::cout << "MOVE message received from player1: " << response->source << ":" << response->dest << "\n";
            if (byteRecv < 0 || byteRecv == 0)
            {
                endMatch = true;
                winner = switchSide(turn);
            }
            else if (response->type != CHESS_MOVE)
            {
                continue;
            }
            else
            {
                if (player1->color == getSide(board[response->source]) && isValidPlayerMove(board, response->source, response->dest))
                {
                    movePiece(board, response->source, response->dest);
                    message1->type = CHESS_MOVE_VALID;
                    message1->source = response->source;
                    message1->dest = response->dest;
                    std::cout << "Set MOVE_VALID message: " << response->source << ":" << response->dest << "\n";
                    sendToClient(player1, message1);
                    sendToClient(player2, message1);
                    std::cout << "MOVE_VALID message sent to both player\n";
                    turn = switchSide(turn);
                } else std::cout << "Invalid player1 move\n";
            }
        }
        else if (player2->color == turn)
        {
            message2->type = CHESS_TURN;
            message1->type = CHESS_WAIT_FOR_TURN;
            sendToClient(player1, message1);
            std::cout << "WAIT_FOR_TURN message sent to player1\n";
            sendToClient(player2, message2);
            std::cout << "TURN message sent to player2\n";
            byteRecv = recv(player2->sockfd, response, sizeof(Message_t), 0);
            std::cout << "MOVE message received from player2: " << response->source << ":" << response->dest << "\n";
            if (byteRecv < 0 || byteRecv == 0)
            {
                endMatch = true;
                winner = switchSide(turn);
            }
            else if (response->type != CHESS_MOVE)
            {
                continue;
            }
            else
            {
                if (player2->color == getSide(board[response->source]) && isValidPlayerMove(board, response->source, response->dest))
                {
                    movePiece(board, response->source, response->dest);
                    message1->type = CHESS_MOVE_VALID;
                    message1->source = response->source;
                    message1->dest = response->dest;
                    sendToClient(player1, message1);
                    sendToClient(player2, message1);
                    std::cout << "Set MOVE_VALID message: " << response->source << ":" << response->dest << "\n";
                    std::cout << "MOVE_VALID message sent to both players\n";
                    turn = switchSide(turn);
                } else std::cout << "Invalid player2 move\n";
            }
        }

        if (applyPromotion(board))
        {
            message1->type == CHESS_BOARD_UPDATE;
            boardCopy(board, message1->primary_message);
            message2->type == CHESS_BOARD_UPDATE;
            boardCopy(board, message2->primary_message);
            sendToClient(player1, message1);
            sendToClient(player2, message2);
            std::cout << "BOARD_UPDATE message sent to player1, player2\n";
        }

        // if checkmate
        if (isBlackMate(board))
        {
            winner = WHITE;
            endMatch = true;
        }
        if (isWhiteMate(board))
        {
            winner = BLACK;
            endMatch = true;
        }
    }

    // Announce winner
    message1->type = CHESS_WINNER;
    message1->success = player1->color == winner ? true : false;
    sendToClient(player1, message1);
    std::cout << "WINNER message sent to player1\n";

    message2->type = CHESS_WINNER;
    message2->success = player2->color == winner ? true : false;
    sendToClient(player2, message2);
    std::cout << "WINNER message sent to player2\n";

    // clean up
    player1->joined = false;
    player2->joined = false;

    room->client1 = nullptr;
    room->client1 = nullptr;
    room->started = false;
    // client_mutex.unlock();

    delete response;
    delete message1;
    delete message2;
    std::cout << "Game finished\n";
}

void login_thread()
{
    int server_fd, client_fd;
    struct sockaddr_in client_address;
    int opt = 1;
    int addrlen = sizeof(client_address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt");
        exit(EXIT_FAILURE);
    }

    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(kPort);

    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&client_address, sizeof(client_address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    std::cout << "Server is now listening on port " << kPort << std::endl;
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    while (running)
    {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept");
            continue;
        }
        else
        {
            std::cout << "New client connected: " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;
            // Check if maximum client connected
            if (client_container.size() < MAX_PLAYER)
            {
                Client_t *new_client = new Client_t;
                new_client->address = client_address;
                new_client->sockfd = client_fd;
                new_client->uid = client_id++;

                // New thread to handle client
                std::thread client_thread(login_handler, new_client);

                // Detach thread
                client_thread.detach();
                // client_container.emplace_back(std::move(client_thread));
            }
            else
            {
                std::cout << "Maximum reached. Client disconnected.\n";
                close(client_fd);
            }
        }
        // Remove disconnected client
        // client_mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void room_create()
{
    // Create game room
    // client_mutex.lock();
    // std::cout.flush();
    // std::cout << "Client container count: " << client_container.size() << "\n";
    // std::printf("Client container count: %ld\n", client_container.size());
    while (true)
    {
        if (client_container.size() == 2)
        {
            if (!client_container[0]->joined && !client_container[1]->joined)
            {
                std::cout << "Creating game room\n";
                game_rooms[0].client1 = client_container[0];
                game_rooms[0].client2 = client_container[1];
                client_container[0]->joined = true;
                client_container[1]->joined = true;
                game_rooms[0].started = true;
                std::thread room_thread(game_room_handler, &game_rooms[0]);
                // room_thread.detach();
                room_thread.join();
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char **argv)
{
    std::thread loginThread(login_thread);
    std::thread roomThread(room_create);
    roomThread.join();
    running = false;
    loginThread.join();
    return 0;
}

// g++ server.cpp ../object/board.o ../object/gamerule.o -o server
