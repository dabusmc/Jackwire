#include "client.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include <SDL3/SDL.h>

#include <stdio.h>

SocketError _initClient(Socket** client, char* ip, char* port)
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
        printf("Usage:\n\tjackwire server <port> <max_players>\n\tjackwire client <port>");
        return -1;
    }

    char* port = argv[2];
    printf("Client attempting connection on port %s...\n", port);

    Socket* client;
    SocketError error = _initClient(&client, "127.0.0.1", port);
    if(error != SOCKET_OK)
    {
        printf("Initializing Network Failed!\n");
        return -1;
    }

    Message msg;
    messageReceive(client, &msg);
    printf("Value Received: %i\n", ((TestMessage*)msg.payload)->value);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        socketDestroy(client);
        return -1;
    }

    SDL_Renderer* renderer;
    SDL_Window* window;
    if (!SDL_CreateWindowAndRenderer("Jackwire", 800, 600, 0, &window, &renderer))
    {
        printf("Couldn't create window/renderer: %s\n", SDL_GetError());
        SDL_Quit();
        socketDestroy(client);
        return -1;
    }

    int running = 1;
    int game_started = 0;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                sendDisconnectMessage(client);
                printf("Disconnected from server!\n");
                running = 0;
            }
        }

        if(socketHasData(client))
        {
            Message msg;
            SocketError error = messageReceive(client, &msg);

            if (error == SOCKET_OK)
            {
                switch(msg.header.type)
                {
                case MESSAGE_S2C_GAME_START:
                    game_started = 1;
                    printf("Game Start!\n");
                    break;
                default:
                    printf("Unimplemented Message: %i\n", msg.header.type);
                    break;
                }
            }

            messageDestroy(&msg);
        }

        if(game_started)
        {
            SDL_SetRenderDrawColorFloat(renderer, 0.14f, 0.14f, 0.14f, SDL_ALPHA_OPAQUE_FLOAT);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
        else
        {
            const double now = ((double)SDL_GetTicks()) / 1000.0;

            const float red = (float) (0.5 + 0.5 * SDL_sin(now));
            const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
            const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

            SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    socketDestroy(client);
    return 0;
}