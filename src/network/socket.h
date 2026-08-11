#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    SOCKET_OK = 0,
    SOCKET_INIT_FAILED,
    SOCKET_CREATION_FAILED,
    SOCKET_ADDRESS_FAILED,
    SOCKET_BIND_FAILED,
    SOCKET_LISTEN_FAILED,
    SOCKET_ACCEPT_FAILED,
    SOCKET_CONNECT_FAILED
} SocketError;

typedef struct Socket Socket;

SocketError socketInit();
void socketCleanup();

SocketError socketCreate(Socket** sock);
SocketError socketBind(Socket* sock, const char* port);
SocketError socketListen(Socket* sock);
SocketError socketAccept(Socket* server, Socket** out_client);
SocketError socketConnect(Socket* sock, const char* address, const char* port);
void socketDestroy(Socket* sock);

#endif