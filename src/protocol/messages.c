#include "messages.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADER_SIZE 8
#define MAX_PAYLOAD_SIZE 1024

#define COPY_TO_OFFSET(buf, offset, val) memcpy((char*)(buf) + (offset), &(val), sizeof(val))
#define COPY_FROM_OFFSET(val, buf, offset) memcpy(&(val), (char*)(buf) + (offset), sizeof(val))

// Serialization
int _messageHeaderSerialize(MessageHeader* header, void* buffer)
{
    uint32_t type = littleToBigEndian(header->type);
    uint32_t length = littleToBigEndian(header->length);

    COPY_TO_OFFSET(buffer, 0, type);
    COPY_TO_OFFSET(buffer, sizeof(type), length);

    return sizeof(type) + sizeof(length);
}

int _testMessageSerialize(TestMessage* message, void* buffer)
{
    uint32_t value = littleToBigEndian(message->value);

    COPY_TO_OFFSET(buffer, 0, value);

    return sizeof(value);
}

int _gameStartMessageSerialize(GameStartMessage* message, void* buffer)
{
    uint32_t temp = littleToBigEndian(message->temp);

    COPY_TO_OFFSET(buffer, 0, temp);

    return sizeof(temp);
}

// Deserialization
int _messageHeaderDeserialize(void* buffer, MessageHeader* header)
{
    uint32_t type;
    COPY_FROM_OFFSET(type, buffer, 0);
    header->type = bigToLittleEndian(type);
    
    uint32_t length;
    COPY_FROM_OFFSET(length, buffer, sizeof(type));
    header->length = bigToLittleEndian(length);

    return sizeof(type) + sizeof(length);
}

int _testMessageDeserialize(void* buffer, TestMessage* message)
{
    uint32_t value;
    COPY_FROM_OFFSET(value, buffer, 0);
    message->value = bigToLittleEndian(value);

    return sizeof(value);
}

int _gameStartMessageDeserialize(void* buffer, GameStartMessage* message)
{
    uint32_t temp;
    COPY_FROM_OFFSET(temp, buffer, 0);
    message->temp = bigToLittleEndian(temp);

    return sizeof(temp);
}

// Life Cycle
void messageDestroy(Message* msg)
{
    if(msg->payload != NULL)
    {
        free(msg->payload);
        msg->payload = NULL;
    }
}

// No-Data Messages
void disconnectMessageCreate(Message* msg)
{
    msg->header.type = MESSAGE_C2S_DISCONNECT;
    msg->header.length = 0;
    msg->payload = NULL;
}

void requestCardsMessageCreate(Message* msg)
{
    msg->header.type = MESSAGE_C2S_REQUEST_CARDS;
    msg->header.length = 0;
    msg->payload = NULL;
}

// Data Messages
void testMessageCreate(Message* msg, int value)
{
    msg->header.type = MESSAGE_TEST;

    TestMessage* test = (TestMessage*)malloc(sizeof(TestMessage));
    test->value = value;

    msg->payload = test;
}

void gameStartMessageCreate(Message* msg, int temp)
{
    msg->header.type = MESSAGE_S2C_GAME_START;

    GameStartMessage* game_start = (GameStartMessage*)malloc(sizeof(GameStartMessage));
    game_start->temp = temp;

    msg->payload = game_start;
}

// Sending
#define ERROR_CHECK() if(error != SOCKET_OK) return error

SocketError messageSend(Socket* socket, Message* msg)
{
    char payload_buffer[MAX_PAYLOAD_SIZE];
    int payload_size = 0;
    switch(msg->header.type)
    {
    case MESSAGE_TEST:
        payload_size = _testMessageSerialize((TestMessage*)(msg->payload), payload_buffer);
        if (msg->payload == NULL)
        {
            return SOCKET_SEND_FAILED;
        }
        msg->header.length = payload_size;
        break;
    case MESSAGE_C2S_DISCONNECT:
        payload_size = 0;
        break;
    case MESSAGE_S2C_GAME_START:
        payload_size = _gameStartMessageSerialize((GameStartMessage*)(msg->payload), payload_buffer);
        if(msg->payload == NULL)
        {
            return SOCKET_SEND_FAILED;
        }
        msg->header.length = payload_size;
        break;
    default:
        return SOCKET_SEND_FAILED;
    }

    char header_buffer[HEADER_SIZE];
    int header_size = _messageHeaderSerialize(&msg->header, header_buffer);

    int bytes_sent = 0;
    SocketError error = socketSend(socket, header_buffer, header_size, &bytes_sent);
    ERROR_CHECK();

    if(payload_size > 0)
    {
        error = socketSend(socket, payload_buffer, payload_size, &bytes_sent);
        ERROR_CHECK();
    }

    return SOCKET_OK;
}

// Receiving
SocketError messageReceive(Socket* socket, Message* msg)
{
    msg->payload = NULL;

    char header_buffer[HEADER_SIZE];
    SocketError error = socketReceiveAll(socket, header_buffer, HEADER_SIZE);
    ERROR_CHECK();
    _messageHeaderDeserialize(header_buffer, &msg->header);

    if (msg->header.length > MAX_PAYLOAD_SIZE)
    {
        return SOCKET_RECV_FAILED;
    }

    char payload_buffer[MAX_PAYLOAD_SIZE];
    error = socketReceiveAll(socket, payload_buffer, msg->header.length);
    ERROR_CHECK();
    
    switch(msg->header.type)
    {
    case MESSAGE_TEST:
        msg->payload = malloc(sizeof(TestMessage));
        _testMessageDeserialize(payload_buffer, msg->payload);
        break;
    case MESSAGE_C2S_DISCONNECT:
        msg->payload = NULL;
        break;
    case MESSAGE_S2C_GAME_START:
        msg->payload = malloc(sizeof(GameStartMessage));
        _gameStartMessageDeserialize(payload_buffer, msg->payload);
        break;
    default:
        return SOCKET_RECV_FAILED;
    }

    return SOCKET_OK;
}