//
// Created by edward on 6/13/20.
//
#include <common.h>
void print_bytes(char *data, int len){
    printf("data:");
    for(int i = 0; i < len; i++){
        printf("%02X", (uint8_t)data[i]);
    }
    printf("\n");
}