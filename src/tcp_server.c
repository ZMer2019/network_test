//
// Created by mingxing yue on 6/3/20.
//


#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT    8712
void tcp_server(uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(ret == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    listen(fd, 1024);
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    char buff[2048] = {0};
    int n;
    while (1){
        int conn_fd = accept(fd,(struct sockaddr*)&client, &len);
        if(conn_fd == -1){
            printf("error:%s, errno=%d\n", strerror(errno), errno);
            exit(1);
        }
        n = read(conn_fd, buff, sizeof(buff) - 1);
        if(n == -1){
            printf("error:%s, errno=%d\n", strerror(errno), errno);
            close(conn_fd);
            continue;
        }
        buff[n] = '\0';
        printf("recv:%s\n", buff);
        n = write(conn_fd, buff, n);
        if(n == -1){
            printf("error:%s, errno=%d\n", strerror(errno), errno);
            close(conn_fd);
        }
        printf("send:%s\n", buff);
        close(conn_fd);
    }
}

int main(int argc, char *argv[]){
    tcp_server(PORT);
    return -1;
}