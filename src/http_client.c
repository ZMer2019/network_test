#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
char *msg = "GET / HTTP/1.1\r\n"
            "Host: 127.0.0.1:8712\r\n"
            "User-Agent: http_client/0.0.1\r\n"
            "Accept: */*\r\n\r\n";

void client(char *addr, uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket");
        return;
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, addr, &server.sin_addr);
    server.sin_port = htons(port);
    server.sin_family = AF_INET;
    int ret = connect(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(ret == -1){
        close(fd);
        perror("connect");
        return;
    }
    write(fd, msg, strlen(msg));
    char buff[4096] = {0};
    read(fd, buff, sizeof(buff) - 1);
    printf("+++%s\n", buff);
    close(fd);
}

int main(int argc, char *argv[]){
    for(int i = 0; i < 2; i++){
        client("127.0.0.1", 8712);
    }
    return 0;
}