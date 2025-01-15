//
// Created by edward on 6/13/20.
//
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TEST_ADDR               "192.168.1.104"

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
    msg.port = htons(9000);
    int n = sendto(fd, &msg, sizeof(struct msg_t), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(n == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    LOGD("sendto:");
    print_bytes((char*)&msg,sizeof(struct msg_t));

    char buff[2048] = {0};
    for(int i = 0; i < 10; i++){
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

}

void send_big_packet(const char *s, uint16_t port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        return;
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    int ret = inet_pton(AF_INET, s, &server.sin_addr);
    if(ret == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    char tmp[8192] = {0};
    for (int i = 0; i < 2048; i++) {
        sprintf(tmp + i*2, "%02x", i+1);
    }
    LOGD("send[%s], length[%lu]\n", tmp, strlen(tmp));
    ssize_t n = sendto(fd, tmp, strlen(tmp),0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(n == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        close(fd);
        return;
    }
    LOGD("send length[%ld]\n", n);
    close(fd);
}

void usage(void) {
    printf("Usage: udp_client [OPTION]... \n");
    printf("Options:\n");
    printf("\t-h, Show this help\n");
    printf("\t-p, set port like 80 ...\n");
    printf("\t-s, set server addr like 192.168.1.100. \n");
    exit(0);
}

int main(int argc, char *argv[]){
    char *server = NULL;
    char *port = NULL;
    char ch;
    char *end = NULL;
    while((ch = getopt(argc, argv, "hs:p:")) != -1) {
        switch(ch) {
            case 's':
                server = optarg;
            break;
            case 'p':
                port = optarg;
            break;
            case 'h':
            default:
                usage();
        }
    }
    if (server == NULL || port == NULL) {
        usage();
    }
    uint16_t dest = strtoul(port, &end, 10);
    if (*end != '\0') {
        LOGD("invalid port[%s]\n", port);
    }else{
        send_big_packet(server, dest);
    }
    return 0;
}
