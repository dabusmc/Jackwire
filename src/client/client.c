#include "client.h"

#include "renderer.h"

#include "network/socket.h"
#include "protocol/messages.h"

#include "game/deck.h"
#include "game/hand.h"

#include <stdio.h>

struct GameData
{
    int game_started;

    Hand* hand;
};
typedef struct GameData GameData;

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

void _dumpCards(Hand* hand)
{
    printf("Cards in Hand\n");
    for(int i = 0; i < hand->card_count; i++)
    {
        uint8_t current = hand->cards[i];
        printf("\t%s of %s\n", cardGetValueName(current), cardGetSuitName(current));
    }
}

int _handleMessage(Message* message, GameData* data)
{
    switch(message->header.type)
    {
    case MESSAGE_S2C_GAME_START:
        GameStartMessage* game_start = (GameStartMessage*)message->payload;
        
        handAddCard(data->hand, game_start->first_card);
        handAddCard(data->hand, game_start->second_card);

        _dumpCards(data->hand);
        
        data->game_started = 1;
        break;
    default:
        printf("Unimplemented Message: %i\n", message->header.type);
        break;
    }

    return 1;
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

    RenderData* renderer;
    RenderError render_error = rendererInit(&renderer, 800, 600, "Jackwire");
    if(render_error != RENDER_OK)
    {
        socketDestroy(client);
        return -1;
    }

    GameData data;
    data.game_started = 0;
    handCreate(&data.hand);

    int running = 1;
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
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_SPACE)
                {
                    printf("Space pressed!\n");
                }
            }
        }

        if(socketHasData(client))
        {
            Message msg;
            SocketError error = messageReceive(client, &msg);

            if (error == SOCKET_OK)
            {
                _handleMessage(&msg, &data);
            }

            messageDestroy(&msg);
        }

        if(data.game_started)
        {
            rendererClear(renderer, 0.14f, 0.14f, 0.14f);
            rendererDisplay(renderer);
        }
        else
        {
            const double now = ((double)SDL_GetTicks()) / 1000.0;

            const float red = (float) (0.5 + 0.5 * SDL_sin(now));
            const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
            const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

            rendererClear(renderer, red, green, blue);
            rendererDisplay(renderer);
        }
    }

    handDestroy(data.hand);
    rendererDestroy(renderer);

    sendDisconnectMessage(client);

    socketDestroy(client);
    return 0;
}