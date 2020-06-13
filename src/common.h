//
// Created by edward on 6/12/20.
//

#ifndef NETWORK_TEST_COMMON_H
#define NETWORK_TEST_COMMON_H
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#define TIME_FORMAT                         "%Y-%m-%d %H:%M:%S"
#define PORT    8712
#define EVENTS_MAX                          1024

#define TEST_MSG                            "Hello, World!"

struct msg_t {
    uint16_t    magic;
    uint16_t    port;
    uint32_t    daddr;
    int32_t     PID;
    int         domainId;
}__attribute__((aligned(1)));

void print_bytes(char *data, int len);

#define _LOG(color, tag, format, ...)                                       \
do{                                                                         \
    time_t now = time(NULL);                                                \
    char timestr[20] = {0};                                                 \
    strftime(timestr, 20, TIME_FORMAT, localtime(&now));                    \
    fprintf(stderr, color " %s %s %d " tag ": \e[0m" format,                \
    timestr, __FUNCTION__, __LINE__, ##__VA_ARGS__);                        \
    fflush(stderr);                                                         \
}while(0)

#define LOGD(format, ...) _LOG("\x1b[32m", "DEBUG", format, ##__VA_ARGS__)
#define LOGE(format, ...) _LOG("\x1b[31m", "ERROR", format, ##__VA_ARGS__)
#endif //NETWORK_TEST_COMMON_H
