#include "server.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include <stdio.h>

#define MAX_PLAYERS 1

int serverMain(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("Usage:\n\tjackwire server <port>\n\tjackwire client <port>");
        return -1;
    }

    char* port = argv[2];

    Socket* server;

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

    Socket* clients[MAX_PLAYERS] = { NULL };
    int client_count = 0;
    while(client_count < MAX_PLAYERS)
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

                    MessageHeader header;
                    TestMessage msg;
                    testMessageCreate(&msg, &header, 42);
                    printf("Value Sent: %i\n", msg.value);
                    
                    error = messageSend(client, &header, &msg);
                    if (error != SOCKET_OK)
                    {
                        printf("Failed to send message.\n");
                    }

                    client_count += 1;
                    
                    break;
                }
            }
        }
    }

    printf("All clients connected.\n");
    socketDestroy(server);

    return 0;
}