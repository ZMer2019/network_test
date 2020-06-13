cmake_minimum_required(VERSION 3.15)
project(network_test C)

set(CMAKE_C_STANDARD 11)
include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(network_test src/http_client.c src/common.h src/common.c)

add_executable(tcp_client src/tcp_client.c src/common.h src/common.c)

add_executable(tcp_server src/tcp_server.c src/common.h src/common.c)

add_executable(udp_client src/udp_client.c src/common.h src/common.c)

add_executable(udp_server src/udp_server.c src/common.h src/common.c)