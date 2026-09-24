cmake_minimum_required(VERSION 3.20)

project(jackwire C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0
)
FetchContent_MakeAvailable(SDL3)

set(SOURCE "src/main.c"
    "src/protocol/messages.c"
    "src/server/server.c"
    "src/client/client.c")

add_executable(jackwire ${SOURCE})
target_include_directories("jackwire" PUBLIC "src")
target_link_libraries(jackwire PRIVATE SDL3::SDL3)

if(WIN32)
    target_sources(jackwire PRIVATE src/network/socket_windows.c)
    target_link_libraries(jackwire PRIVATE ws2_32)
elseif(UNIX)
    target_sources(jackwire PRIVATE src/network/socket_linux.c)
endif()

add_custom_command(TARGET jackwire POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:jackwire>
)