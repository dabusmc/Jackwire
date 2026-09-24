#include "client.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include <stdio.h>

SocketError _initNetwork(Socket** client, char* ip, char* port)
{
    SocketError error = socketCreate(client);
    if(error != SOCKET_OK)
    {
        return error;
    }

    error = socketConnect(*client, ip, port);
    if(error != SOCKET_OK)
    {
        socketDestroy(*client);
        return error;
    }

    printf("Connected to server!\n");
    return SOCKET_OK;
}

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
    SocketError error = _initNetwork(&client, "127.0.0.1", port);
    if(error != SOCKET_OK)
    {
        printf("Initializing Network Failed!\n");
        return -1;
    }

    MessageHeader header;
    TestMessage msg;
    messageReceive(client, &header, &msg);
    printf("Value Received: %i\n", msg.value);

    socketDestroy(client);

    return 0;
}