cmake_minimum_required(VERSION 3.15)
project(network_test C)

set(CMAKE_C_STANDARD 11)

add_executable(network_test src/http_client.c )

add_executable(tcp_client src/tcp_client.c)

add_executable(tcp_server src/tcp_server.c)
