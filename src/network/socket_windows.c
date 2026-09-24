#include "network/socket.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>

struct Socket
{
    SOCKET impl;
};

SocketError socketInit()
{
    WSADATA data;
    
    int result = WSAStartup(MAKEWORD(2, 2), &data);
    if(result != 0)
    {
        return SOCKET_INIT_FAILED;
    }

    return SOCKET_OK;
}

void socketCleanup()
{
    WSACleanup();
}

SocketError socketCreate(Socket** sock)
{
    Socket* s = malloc(sizeof(Socket));
    if (s == NULL)
    {
        return SOCKET_CREATION_FAILED;
    }

    s->impl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s->impl == INVALID_SOCKET)
    {
        free(s);
        return SOCKET_CREATION_FAILED;
    }

    *sock = s;

    return SOCKET_OK;
}

SocketError socketBind(Socket* sock, const char* port)
{
    struct addrinfo hints;
    struct addrinfo* result = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port, &hints, &result);
    if (status != 0)
    {
        return SOCKET_ADDRESS_FAILED;
    }

    status = bind(sock->impl, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    if (status == SOCKET_ERROR)
    {
        return SOCKET_BIND_FAILED;
    }

    return SOCKET_OK;
}

SocketError socketListen(Socket* sock)
{
    int result = listen(sock->impl, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        return SOCKET_LISTEN_FAILED;
    }

    return SOCKET_OK;
}

SocketError socketAccept(Socket* server, Socket** out_client)
{
    Socket *client = malloc(sizeof(Socket));

    if (client == NULL)
    {
        return SOCKET_ACCEPT_FAILED;
    }

    client->impl = accept(server->impl, NULL, NULL);
    if (client->impl == INVALID_SOCKET)
    {
        free(client);
        return SOCKET_ACCEPT_FAILED;
    }

    *out_client = client;

    return SOCKET_OK;
}

SocketError socketConnect(Socket* sock, const char* address, const char* port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(address, port, &hints, &result);
    if (status != 0)
    {
        return SOCKET_ADDRESS_FAILED;
    }

    status = connect(sock->impl, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    if (status == SOCKET_ERROR)
    {
        return SOCKET_CONNECT_FAILED;
    }

    return SOCKET_OK;
}

void socketDestroy(Socket* sock)
{
    if (sock == NULL)
    {
        return;
    }

    closesocket(sock->impl);
    free(sock);
}

SocketError socketSend(Socket *sock, const void *data, int length)
{
    int result = send(sock->impl, data, length, 0);
    
    if (result == SOCKET_ERROR)
    {
        return SOCKET_SEND_FAILED;
    }

    return SOCKET_OK;
}

SocketError socketReceive(Socket *sock, void *buffer, int length)
{
    int result = recv(sock->impl, buffer, length, 0);

    if(result == SOCKET_ERROR)
    {
        return SOCKET_RECV_FAILED;
    }

    return SOCKET_OK;
}