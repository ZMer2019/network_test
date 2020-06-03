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
#define PORT        8712
#define ADDR        "127.0.0.1"
char *msg = "Hello,World!";
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
    int n = write(fd, msg, strlen(msg));
    if(n == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    printf("send:%s\n", msg);
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
    tcp_client(ADDR, PORT);
    return 0;
}