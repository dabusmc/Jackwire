#include <stdio.h>
#include <string.h>

#include "network/socket.h"

#include "server/server.h"
#include "client/client.h"

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        printf("Usage:\n\tjackwire server <port> <max_players>\n\tjackwire client <port>");
        return -1;
    }

    char* mode = argv[1];
    
    int init = socketInit();
    if(init == SOCKET_INIT_FAILED)
    {
        printf("Socket Init Failed.\n");
        return -1;
    }

    int result = 0;
    if(strcmp(mode, "server") == 0)
    {
        result = serverMain(argc, argv);
    }
    else if(strcmp(mode, "client") == 0)
    {
        result = clientMain(argc, argv);
    }
    else
    {
        printf("Usage:\n\tjackwire server <port> <max_players>\n\tjackwire client <port>");
        result = -1;
    }

    socketCleanup();
    return result;
}