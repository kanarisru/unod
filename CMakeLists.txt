cmake_minimum_required(VERSION 3.16)
project(unod2)

set(CMAKE_CXX_STANDARD 14)
link_libraries(ws2_32 wsock32)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
add_executable(unod2 main.cpp inc/socket_server.h inc/socket_server.cpp inc/socket_client.h inc/socket_client.cpp inc/func.h inc/func.cpp)