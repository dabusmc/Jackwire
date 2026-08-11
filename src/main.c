#include <stdio.h>
#include <string.h>

#include "network/socket.h"

int serverMain(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("Usage: dhwahgduiywah");
        return -1;
    }

    char* port = argv[2];

    Socket* server;
    Socket* client;

    SocketError error = socketCreate(&server);
    if(error != SOCKET_OK)
    {
        printf("Socket creation failed!\n");
        return -1;
    }

    error = socketBind(server, port);
    if(error != SOCKET_OK)
    {
        printf("Socket binding failed!\n");
        socketDestroy(server);
        return -1;
    }

    printf("Socket bound to port %s.\n", port);

    error = socketListen(server);
    if(error != SOCKET_OK)
    {
        printf("Socket listening failed!\n");
        socketDestroy(server);
        return -1;
    }

    printf("Listening for connections...\n");

    error = socketAccept(server, &client);
    if(error != SOCKET_OK)
    {
        printf("Socket accepting failed!\n");
        socketDestroy(server);
        return -1;
    }

    printf("Client connected successfully\n");

    socketDestroy(client);
    socketDestroy(server);

    return 0;
}

int clientMain(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("Usage: dhwahgduiywah");
        return -1;
    }

    char* port = argv[2];

    printf("Client attempting connection on port %s...\n", port);

    Socket* client;
    SocketError error = socketCreate(&client);
    if(error != SOCKET_OK)
    {
        printf("Socket creation failed!\n");
        return -1;
    }
    
    error = socketConnect(client, "127.0.0.1", port);
    if(error != SOCKET_OK)
    {
        printf("Socket connection failed!\n");
        socketDestroy(client);
        return -1;
    }

    printf("Connected to server!\n");
    socketDestroy(client);

    return 0;
}

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf("Usage: dhwahgduiywah");
        return -1;
    }

    char* mode = argv[1];
    
    int init = socketInit();
    if(init == SOCKET_INIT_FAILED)
    {
        printf("Socket Init Failed.\n");
        return -1;
    }

    int result = 0;
    if(strcmp(mode, "server") == 0)
    {
        result = serverMain(argc, argv);
    }
    else if(strcmp(mode, "client") == 0)
    {
        result = clientMain(argc, argv);
    }
    else
    {
        printf("Usage: dhwahgduiywah");
        result = -1;
    }

    socketCleanup();
    return result;
}