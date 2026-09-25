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
    uint8_t first_card;
    uint8_t second_card;
};
typedef struct GameStartMessage GameStartMessage;

#define sendDatalessMessage(socket, func) \
    Message __msg__; \
    func(&__msg__); \
    messageSend(socket, &__msg__); \
    messageDestroy(&__msg__)

// Life Cycle
void messageDestroy(Message* msg);

// No-Data Messages
void disconnectMessageCreate(Message* msg);
#define sendDisconnectMessage(socket) sendDatalessMessage(socket, disconnectMessageCreate)

void requestCardsMessageCreate(Message* msg);
#define sendRequestCardsMessage(socket) sendDatalessMessage(socket, requestCardsMessageCreate)

// Data Messages
void testMessageCreate(Message* msg, int value);
void gameStartMessageCreate(Message* msg, uint8_t first_card, uint8_t second_card);

// Sending
SocketError messageSend(Socket* socket, Message* msg);

// Receiving
SocketError messageReceive(Socket* socket, Message* msg);

#endif