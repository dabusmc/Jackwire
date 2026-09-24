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

// Creation
void testMessageCreate(TestMessage* message, MessageHeader* header, int value);
void disconnectMessageCreate(MessageHeader* header);

// Sending
SocketError messageSend(Socket* socket, MessageHeader* header, void* payload);

// Receiving
SocketError messageReceive(Socket* socket, MessageHeader* header, void* payload);

#endif