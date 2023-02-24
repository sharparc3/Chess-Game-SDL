#include "login.h"

void on_button_clicked(GtkWidget *widget, gpointer data)
{
    // Cast the data parameter to a pointer to the LoginData structure
    LoginData *widgetData = (LoginData *)data;

    // Get the text from the username and password text boxes
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(widgetData->loginWindow->textboxUser));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(widgetData->loginWindow->textboxPass));

    // Print the username and password to the console
    // g_print("Username: %s\nPassword: %s\n", username, password);

    // store the value
    // widgetData->user = std::string(username);
    // widgetData->pass = std::string(password);

    Message_t *loginMsg = new Message_t;
    Message_t *loginRsp = new Message_t;

    // Connect to server
    // if (!widgetData->connected)
    // {
    //     int status = connect(widgetData->client_fd, (struct sockaddr *)&widgetData->server_address, sizeof(widgetData->server_address));
    //     if (status < 0)
    //     {
    //         std::perror("Connection failed");
    //     }
    // }

    widgetData->client_fd = new int;

    // Initialize socket
    *widgetData->client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*widgetData->client_fd < 0)
    {
        std::cerr << "Could not create socket" << std::endl;
        return;
    }

    if (connect(*widgetData->client_fd, (struct sockaddr *)&widgetData->server_address, sizeof(widgetData->server_address)) < 0)
    {
        std::perror("Connection failed");
    }
    else
    {
        widgetData->connected = true;

        // send login message
        loginMsg->type = CHESS_LOGIN;
        std::strcpy(loginMsg->primary_message, username);
        std::strcpy(loginMsg->secondary_message, password);

        // send login
        send(*widgetData->client_fd, loginMsg, sizeof(Message_t), 0);
        recv(*widgetData->client_fd, loginRsp, sizeof(Message_t), 0);
        if (loginRsp->type == CHESS_LOGIN_RESPONSE && loginRsp->success)
        {
            widgetData->loggedIn = true;
            widgetData->user = std::string(username);
            std::cout << "Login successfully\n";
        }
        else
        if (loginRsp->type == CHESS_LOGIN_RESPONSE && !loginRsp->success)
        {
            std::cout << "Username or password not matched\n";
            close(*widgetData->client_fd);
            delete widgetData->client_fd;
            widgetData->client_fd = nullptr;
        }
    }

    if (widgetData->loggedIn)
    {
        gtk_widget_destroy(widgetData->loginWindow->textboxUser);
        gtk_widget_destroy(widgetData->loginWindow->textboxPass);
        gtk_widget_destroy(widgetData->loginWindow->bigLabelLogin);
        gtk_widget_destroy(widgetData->loginWindow->image);
        gtk_widget_destroy(widgetData->loginWindow->labelPass);
        gtk_widget_destroy(widgetData->loginWindow->labelUser);
        gtk_widget_destroy(widgetData->loginWindow->buttonLogin);
        gtk_widget_destroy(widgetData->loginWindow->fixed);
        gtk_widget_destroy(widgetData->loginWindow->window);
        delete loginMsg;
        delete loginRsp;
    }
}

void loginScreen(LoginData *data)
{
    GtkComponents loginWindow;
    // Create the window
    loginWindow.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Set the size of the window
    gtk_window_set_default_size(GTK_WINDOW(loginWindow.window), 600, 300);

    // Make the window non-resizable
    gtk_window_set_resizable(GTK_WINDOW(loginWindow.window), FALSE);

    // Set label
    gtk_window_set_title(GTK_WINDOW(loginWindow.window), "Login");

    // Set the background color of the window using CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "window { background-color: #ddd6d5; }", -1, NULL);
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Create a fixed container
    loginWindow.fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(loginWindow.window), loginWindow.fixed);

    // Load the image
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("../asset/main.png", NULL);
    if (pixbuf == nullptr)
    {
        std::cout << "Failed to load image\n";
        return;
    }
    loginWindow.image = gtk_image_new_from_pixbuf(pixbuf);

    // Set the position of the image
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.image, 0, 0);

    // Create a new GtkLabel widget
    loginWindow.bigLabelLogin = gtk_label_new("LOGIN");
    // Set the position of the label
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.bigLabelLogin, 365, 20);
    // Create a new PangoAttrList
    PangoAttrList *attr_list = pango_attr_list_new();
    // Create a new PangoAttrSize
    PangoAttribute *attr_size = (PangoAttribute *)pango_attr_size_new_absolute(40 * PANGO_SCALE);
    // Add the PangoAttrSize to the PangoAttrList
    pango_attr_list_insert(attr_list, attr_size);
    // Create a new PangoAttrWeight
    PangoAttribute *attr_weight = (PangoAttribute *)pango_attr_weight_new(PANGO_WEIGHT_BOLD);
    // Add the PangoAttrWeight to the PangoAttrList
    pango_attr_list_insert(attr_list, attr_weight);
    // Set the PangoAttrList for the label
    gtk_label_set_attributes(GTK_LABEL(loginWindow.bigLabelLogin), attr_list);
    // Free the PangoAttrList and PangoAttrWeight
    pango_attr_list_unref(attr_list);
    g_object_unref(attr_weight);
    // Create a new GdkRGBA object with the desired color
    GdkRGBA color;
    gdk_rgba_parse(&color, "#964D19");
    // Set the foreground color of the label
    gtk_widget_override_color(loginWindow.bigLabelLogin, GTK_STATE_FLAG_NORMAL, &color);

    // Create username label widget
    loginWindow.labelUser = gtk_label_new("Username");
    // Set the position of the label
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.labelUser, 300, 85);

    // Create username label widget
    loginWindow.labelPass = gtk_label_new("Password");
    // Set the position of the label
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.labelPass, 300, 165);

    // Create username text box
    loginWindow.textboxUser = gtk_entry_new();
    gtk_widget_set_size_request(loginWindow.textboxUser, 250, 30);
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.textboxUser, 300, 110);

    // Create password text box
    loginWindow.textboxPass = gtk_entry_new();
    gtk_widget_set_size_request(loginWindow.textboxPass, 250, 30);
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.textboxPass, 300, 190);
    // Set the visibility of the entry to FALSE
    gtk_entry_set_visibility(GTK_ENTRY(loginWindow.textboxPass), FALSE);
    // Set the invisible character of the entry to '*'
    gtk_entry_set_invisible_char(GTK_ENTRY(loginWindow.textboxPass), '*');

    // Create a login button widget
    loginWindow.buttonLogin = gtk_button_new_with_label("Login");
    // Set the position of the button
    gtk_fixed_put(GTK_FIXED(loginWindow.fixed), loginWindow.buttonLogin, 300, 245);
    // Set the button background color
    GtkCssProvider *button_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(button_provider, "button { background-color: #d4a682; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(loginWindow.buttonLogin), GTK_STYLE_PROVIDER(button_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    data->loginWindow = &loginWindow;

    g_signal_connect(loginWindow.buttonLogin, "clicked", G_CALLBACK(on_button_clicked), data);

    // Quit the application when the window is closed
    g_signal_connect(loginWindow.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show the window
    gtk_widget_show_all(loginWindow.window);

    // Start the GTK main loop
    gtk_main();
}

// linking: `pkg-config --cflags --libs gtk+-3.0`