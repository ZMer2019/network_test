//
// Created by edward on 6/12/20.
//

#ifndef NETWORK_TEST_COMMON_H
#define NETWORK_TEST_COMMON_H
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#define TIME_FORMAT                         "%Y-%m-%d %H:%M:%S"
#define PORT    8712
#define EVENTS_MAX                          1024

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
