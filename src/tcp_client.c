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
#define PORT        9000
#define ADDR        "127.0.0.1"

#define HELLOWORLD  "hello world"
struct msg_t {
    uint16_t    magic;
    uint16_t    port;
    uint32_t    daddr;
    int32_t     PID;
    int         domainId;
}__attribute__((aligned(1)));
void print_bytes(char *data, int len){
    printf("data:");
    for(int i = 0; i < len; i++){
        printf("%02X", (uint8_t)data[i]);
    }
    printf("\n");
}
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
    char sendBuff[1024] = {0};
    struct msg_t msg;
    msg.magic = 0x9527;
    msg.port = htons(8712);
    struct in_addr ia;
    inet_pton(AF_INET, ADDR, &ia);
    msg.daddr = ia.s_addr;
    msg.PID = 9999;
    msg.domainId = 19;
    int msg_len = sizeof(struct msg_t);
    printf("msg_len=%d\n", msg_len);
    memcpy(sendBuff, (void*)&msg, msg_len);
    memcpy(sendBuff + msg_len, HELLOWORLD, strlen(HELLOWORLD));
    int sum = msg_len + strlen(HELLOWORLD);
    int n = write(fd, sendBuff, sum);
    if(n == -1){
        printf("error:%s, errno=%d\n", strerror(errno), errno);
        exit(1);
    }
    print_bytes(sendBuff, sum);
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