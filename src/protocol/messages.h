#ifndef MESSAGES_H
#define MESSAGES_H

#include "network/socket.h"

#include <stdint.h>

typedef enum
{
    // Two-Way Messages
    MESSAGE_TEST = 0,

    // C2S Messages
    MESSAGE_C2S_DISCONNECT,
    MESSAGE_C2S_REQUEST_CARDS,

    // S2C Messages
    MESSAGE_S2C_GAME_START,
} MessageType;

struct MessageHeader
{
    MessageType type;
    uint32_t length;
};
typedef struct MessageHeader MessageHeader;

struct Message
{
    MessageHeader header;
    void* payload;
};
typedef struct Message Message;

struct TestMessage
{
    uint32_t value;
};
typedef struct TestMessage TestMessage;

struct GameStartMessage
{
    uint32_t temp;
};
typedef struct GameStartMessage GameStartMessage;

#define sendDatalessMessage(socket, func) \
    Message msg; \
    func(&msg); \
    messageSend(socket, &msg); \
    messageDestroy(&msg)

// Life Cycle
void messageDestroy(Message* msg);

// No-Data Messages
void disconnectMessageCreate(Message* msg);
#define sendDisconnectMessage(socket) sendDatalessMessage(socket, disconnectMessageCreate)

void requestCardsMessageCreate(Message* msg);
#define sendRequestCardsMessage(socket) sendDatalessMessage(socket, requestCardsMessageCreate)

// Data Messages
void testMessageCreate(Message* msg, int value);
void gameStartMessageCreate(Message* msg, int temp);

// Sending
SocketError messageSend(Socket* socket, Message* msg);

// Receiving
SocketError messageReceive(Socket* socket, Message* msg);

#endif