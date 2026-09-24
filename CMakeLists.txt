cmake_minimum_required(VERSION 3.20)

project(jackwire C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCE "src/main.c"
    "src/protocol/messages.c"
    "src/server/server.c"
    "src/client/client.c")

add_executable(jackwire ${SOURCE})
target_include_directories("jackwire" PUBLIC "src")

if(WIN32)
    target_sources(jackwire PRIVATE src/network/socket_windows.c)
    target_link_libraries(jackwire PRIVATE ws2_32)
elseif(UNIX)
    target_sources(jackwire PRIVATE src/network/socket_linux.c)
endif()