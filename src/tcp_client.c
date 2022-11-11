//
// Created by mingxing yue on 6/3/20.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <common.h>


#define HELLOWORLD  "hello world"


void tcp_client(const char *addr, uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        return;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    int ret = inet_pton(AF_INET, addr, &server_addr.sin_addr);
    if(ret == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    server_addr.sin_port = htons(port);
    ret = connect(fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
    if(ret == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }


    int n = write(fd, HELLOWORLD, strlen(HELLOWORLD));
    if(n == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    printf("sum=%d\n", n);
    print_bytes(HELLOWORLD, n);

    char buff[2048] = {0};
    n = read(fd, buff, sizeof(buff) - 1);
    if(n == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    buff[n] = '\0';
    printf("recv:%s\n", buff);
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Example: %s 192.168.1.100 9999\n", argv[0]);
        return 0;
    }
    int port = atoi(argv[2]);
    tcp_client(argv[1], port);
    return 0;
}