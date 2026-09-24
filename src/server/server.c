#include "server.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include <stdio.h>

#define MAX_PLAYERS 1

SocketError _initServer(Socket** server, char* port)
{
    SocketError error = socketCreate(server);
    if(error != SOCKET_OK)
    {
        return error;
    }

    error = socketBind(*server, port);
    if(error != SOCKET_OK)
    {
        socketDestroy(*server);
        return error;
    }

    printf("Socket bound to port %s.\n", port);

    error = socketListen(*server);
    if(error != SOCKET_OK)
    {
        socketDestroy(*server);
        return error;
    }

    return SOCKET_OK;
}

SocketError _waitForClients(Socket* server, Socket** clients, int* client_count)
{
    printf("Listening for connections...\n");

    int connected_client_count = 0;
    while(connected_client_count < MAX_PLAYERS)
    {
        Socket* client;

        if(socketAccept(server, &client) == SOCKET_OK)
        {
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (clients[i] == NULL)
                {
                    clients[i] = client;
                    printf("Client %d connected.\n", i + 1);

                    Message msg;
                    testMessageCreate(&msg, 42);
                    printf("Value Sent: %i\n", ((TestMessage*)msg.payload)->value);
                    
                    SocketError error = messageSend(client, &msg);
                    if (error != SOCKET_OK)
                    {
                        return error;
                    }
                    
                    messageDestroy(&msg);

                    connected_client_count += 1;
                    
                    break;
                }
            }
        }
    }

    *client_count = connected_client_count;
    printf("All clients connected.\n");

    return SOCKET_OK;
}

int serverMain(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("Usage:\n\tjackwire server <port>\n\tjackwire client <port>");
        return -1;
    }

    char* port = argv[2];

    Socket* server;
    SocketError error = _initServer(&server, port);
    if(error != SOCKET_OK)
    {
        printf("Initializing Network Failed!\n");
        return -1;
    }

    Socket* clients[MAX_PLAYERS] = { NULL };
    int client_count = 0;
    _waitForClients(server, clients, &client_count);

    int running = 1;
    int ready[MAX_PLAYERS] = { 0 };
    while(running)
    {
        SocketError error = socketSelect(clients, client_count, ready);
        if(error != SOCKET_OK)
        {
            printf("Socket select failed!\n");
            break;
        }

        for(int i = 0; i < client_count; i++)
        {
            if(ready[i] == 1)
            {
                Message msg;
                messageReceive(clients[i], &msg);

                if(msg.header.type == MESSAGE_DISCONNECT)
                {
                    running = 0;
                }
            }
        }
    }

    socketDestroy(server);

    return 0;
}