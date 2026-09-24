#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>

typedef enum
{
    SOCKET_OK = 0,
    SOCKET_INIT_FAILED,
    SOCKET_CREATION_FAILED,
    SOCKET_ADDRESS_FAILED,
    SOCKET_BIND_FAILED,
    SOCKET_LISTEN_FAILED,
    SOCKET_ACCEPT_FAILED,
    SOCKET_CONNECT_FAILED,

    SOCKET_SEND_FAILED,
    SOCKET_RECV_FAILED,

    SOCKET_CONNECTION_CLOSED
} SocketError;

typedef struct Socket Socket;

// Life Cycle
SocketError socketInit();
void socketCleanup();

SocketError socketCreate(Socket** sock);
SocketError socketBind(Socket* sock, const char* port);
SocketError socketListen(Socket* sock);
SocketError socketAccept(Socket* server, Socket** out_client);
SocketError socketConnect(Socket* sock, const char* address, const char* port);
void socketDestroy(Socket* sock);

// Send/Receive
SocketError socketSend(Socket* sock, const void* data, int target_length, int* bytes_sent);
SocketError socketReceive(Socket* sock, void* buffer, int target_length, int* bytes_received);
SocketError socketReceiveAll(Socket* sock, void* buffer, int target_length);

// Control
SocketError socketSelect(Socket** sockets, int socket_count, int* ready);
int socketHasData(Socket *socket);

// Utility
uint32_t littleToBigEndian(uint32_t value);
uint32_t bigToLittleEndian(uint32_t value);

#endif