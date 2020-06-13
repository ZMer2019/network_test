//
// Created by edward on 6/13/20.
//
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TEST_ADDR               "192.168.10.23"

void client(const char* addr, uint16_t port){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        return;
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    int ret = inet_pton(AF_INET, addr, &server.sin_addr);
    if(ret == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    struct msg_t msg;
    msg.magic = htons(9527);
    msg.domainId = htonl(1987);
    msg.PID = htonl(2020);
    struct in_addr ia;
    inet_pton(AF_INET, TEST_ADDR, &ia);
    msg.daddr = ia.s_addr;
    msg.port = htons(9809);
    int n = sendto(fd, &msg, sizeof(struct msg_t), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(n == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    LOGD("sendto:");
    print_bytes((char*)&msg,sizeof(struct msg_t));

    char buff[2048] = {0};
    memcpy(buff, TEST_MSG, strlen(TEST_MSG));
    n = sendto(fd, buff, strlen(TEST_MSG),0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(n == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    LOGD("sendto:%s\n", buff);
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    n = recvfrom(fd, buff, sizeof(buff)-1, 0, (struct sockaddr*)&client, &len);
    if(n == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    buff[n] = '\0';
    LOGD("recvfrom:%s\n", buff);
}

int main(int argc, char *argv[]){
    client("127.0.0.1", 9000);
    return 0;
}