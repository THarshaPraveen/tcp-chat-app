cmake_minimum_required(VERSION 3.10)

project(chatapp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(SocketUtil)
add_subdirectory(SocketClient)
add_subdirectory(SocketServer)

