#ifndef MESSAGES_H
#define MESSAGES_H

#include "network/socket.h"

#include <stdint.h>

typedef enum
{
    MESSAGE_TEST = 0,
    MESSAGE_DISCONNECT
} MessageType;

struct MessageHeader
{
    MessageType type;
    uint32_t length;
};

typedef struct MessageHeader MessageHeader;

struct TestMessage
{
    uint32_t value;
};

typedef struct TestMessage TestMessage;

struct Message
{
    MessageHeader header;
    void* payload;
};

typedef struct Message Message;

#define sendDisconnectMessage(socket) \
    Message msg; \
    disconnectMessageCreate(&msg); \
    messageSend(socket, &msg); \
    messageDestroy(&msg)

// Life Cycle
void testMessageCreate(Message* msg, int value);
void disconnectMessageCreate(Message* msg);
void messageDestroy(Message* msg);

// Sending
SocketError messageSend(Socket* socket, Message* msg);

// Receiving
SocketError messageReceive(Socket* socket, Message* msg);

#endif