//
// Created by edward on 6/13/20.
//
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
void server_loop(uint16_t port){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        return;
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if(ret == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        goto err_exit;
    }
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    int n;
    char buff[2048];
    while(1){
        n = recvfrom(fd, buff, sizeof(buff)-1, 0, (struct sockaddr*)&client, &len);
        if( n == -1){
            LOGE("%s, errno = %d\n", strerror(errno), errno);
            continue;
        }
        LOGD("recv:");
        print_bytes(buff, n);
        buff[n] = '\0';
        LOGD("%s\n", buff);
        n = sendto(fd, buff, n, 0, (struct sockaddr*)&client,sizeof(struct sockaddr_in));
        if( n == -1){
            LOGE("%s, errno = %d\n", strerror(errno), errno);
            continue;
        }
        LOGD("send:");
        print_bytes(buff, n);
        LOGD("%s\n", buff);
    }
err_exit:
    close(fd);
}

int main(int argc, char *argv[]){
    server_loop(9000);
    return 0;
}
