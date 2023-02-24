#ifndef LOGIN_H
#define LOGIN_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/message.h"

typedef struct
{
    GtkWidget *textboxUser;
    GtkWidget *textboxPass;
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *fixed;
    GtkWidget *labelUser, *labelPass, *bigLabelLogin;
    GtkWidget *buttonLogin;
    GMainLoop *loop;
} GtkComponents;

// Structure to hold the widgets
typedef struct
{
    GtkComponents *loginWindow;
    std::string user;
    std::string pass;
    int *client_fd = nullptr;
    struct sockaddr_in server_address;
    bool connected = false;
    bool loggedIn = false;
} LoginData;


// This function is called when the button is clicked
void on_button_clicked(GtkWidget *widget, gpointer data);

// Render login screen
void loginScreen(LoginData *data);

#endif