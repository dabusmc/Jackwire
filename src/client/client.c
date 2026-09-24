#include "client.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include <stdio.h>

int clientMain(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("Usage:\n\tjackwire server <port>\n\tjackwire client <port>");
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

    MessageHeader header;
    TestMessage msg;
    messageReceive(client, &header, &msg);
    printf("Value Received: %i\n", msg.value);

    socketDestroy(client);

    return 0;
}