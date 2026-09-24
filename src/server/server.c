#include "server.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include "game/deck.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

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

SocketError _waitForClients(Socket* server, int max_players, Socket** clients, int* client_count)
{
    printf("Listening for connections...\n");

    int connected_client_count = 0;
    while(connected_client_count < max_players)
    {
        Socket* client;

        if(socketAccept(server, &client) == SOCKET_OK)
        {
            for (int i = 0; i < max_players; i++)
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
    if(argc <= 3)
    {
        printf("Usage:\n\tjackwire server <port> <max_players>\n\tjackwire client <port>");
        return -1;
    }

    char* port = argv[2];
    int max_players = atoi(argv[3]);

    srand((unsigned int)time(NULL));

    Socket* server;
    SocketError error = _initServer(&server, port);
    if(error != SOCKET_OK)
    {
        printf("Initializing Network Failed!\n");
        return -1;
    }

    printf("Max Players: %i\n", max_players);
    
    int client_count = 0;
    Socket** clients = malloc(sizeof(Socket*) * max_players);
    for (int i = 0; i < max_players; i++)
    {
        clients[i] = NULL;
    }
    _waitForClients(server, max_players, clients, &client_count);

    int* ready = malloc(sizeof(int) * max_players);
    for (int i = 0; i < max_players; i++)
    {
        ready[i] = 0;
    }

    // Game Init
    Deck* deck = NULL;
    deckCreate(&deck);
    if(deck == NULL)
    {
        printf("Failed to initialize deck!\n");
        socketDestroy(server);
        return -1;
    }

    for(int i = 0; i < 10; i++)
    {
        uint8_t drawn = deckDrawNext(deck);

        printf("Drew: %s of %s (", cardGetValueName(drawn), cardGetSuitName(drawn));
        for (int i = 7; i >= 0; i--)
        {
            printf("%d ", (drawn >> i) & 1);
        }
        printf(")\n");
    }

    Message game_start_message;
    for(int i = 0; i < max_players; i++)
    {
        gameStartMessageCreate(&game_start_message, i);
        SocketError error = messageSend(clients[i], &game_start_message);        
        if (error != SOCKET_OK)
        {
            printf("Failed to send game start to client %d. Error code: %i\n", i, error);
        }
        messageDestroy(&game_start_message);
    }
    printf("Game Start!\n");

    int running = 1;
    while(running)
    {
        SocketError error = socketSelect(clients, max_players, ready);
        if(error != SOCKET_OK)
        {
            printf("Socket select failed!\n");
            break;
        }

        for(int i = 0; i < max_players; i++)
        {
            if(ready[i] == 1)
            {
                Message msg;
                SocketError error = messageReceive(clients[i], &msg);

                if(error != SOCKET_OK)
                {
                    if(error == SOCKET_CONNECTION_CLOSED)
                    {
                        messageDestroy(&msg);
                        socketDestroy(clients[i]);
                        clients[i] = NULL;
                        client_count--;

                        printf("Client %i disconnected!\n", i);

                        continue;
                    }

                    printf("Error!\n");
                    messageDestroy(&msg);
                    socketDestroy(server);
                    return -1;
                }

                if(msg.header.type == MESSAGE_C2S_DISCONNECT)
                {
                    messageDestroy(&msg);
                    socketDestroy(clients[i]);
                    clients[i] = NULL;
                    client_count--;

                    printf("Client %i disconnected!\n", i);

                    continue;
                }

                messageDestroy(&msg);
            }
        }

        if(client_count == 0)
        {
            running = 0;
        }
    }

    // Game Cleanup
    deckDestroy(deck);

    // Server Cleanup
    free(clients);
    free(ready);
    socketDestroy(server);

    return 0;
}