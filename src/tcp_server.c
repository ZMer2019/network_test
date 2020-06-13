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
#include <common.h>
#include <sys/epoll.h>
#include <fcntl.h>


int server_init(uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        LOGE("%s, errno=%d\n", strerror(errno), errno);
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(ret == -1){
        close(fd);
        LOGE("error:%s, errno=%d\n", strerror(errno), errno);
        return -1;
    }
    ret = listen(fd, 1024);
    if(ret == -1){
        close(fd);
        LOGE("error:%s, errno=%d\n", strerror(errno), errno);
        return -1;
    }
    return fd;
}

int add_event(int epfd, int fd){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if(ret == -1){
        LOGE("%s, errno=%d\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}
int del_event(int epfd, int fd){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
    if(ret == -1){
        LOGE("%s, errno=%d\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}
int setnonblocking(int fd){
    int flag = fcntl(fd, F_GETFL);
    if(flag == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        return -1;
    }
    flag |= O_NONBLOCK;
    flag = fcntl(fd, F_SETFL, flag);
    if(flag == -1){
        LOGE("%s, errno = %d\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}
void server_loop(int listenfd){
    int epfd = epoll_create(1);
    if(epfd == -1){
        LOGE("%s, errno=%d\n", strerror(errno), errno);
        return;
    }
    int conn, n, fd, nfds;
    if(add_event(epfd, listenfd) != 0){
        return;
    }
    struct epoll_event events[EVENTS_MAX];
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    char buff[2048] = {0};
    char ipstr[INET_ADDRSTRLEN] = {0};
    LOGD("server start, listen:0.0.0.0:%u\n", PORT);
    while(1){
        nfds = epoll_wait(epfd, events, EVENTS_MAX, -1);
        if(nfds == -1){
            LOGE("%s, errno=%d\n", strerror(errno), errno);
            LOGD("server exit\n");
            exit(1);
        }
        for (int i = 0; i < nfds; i++){
            if(events[i].data.fd == listenfd){
                conn = accept(listenfd, (struct sockaddr*)&client, &len);
                if(conn == -1){
                    LOGE("%s, errno = %d\n", strerror(errno), errno);
                    continue;
                }
                if(setnonblocking(conn) != 0){
                    close(conn);
                    continue;
                }
                add_event(epfd, conn);
            }else{
                fd = events[i].data.fd;
                n = read(fd, buff, sizeof(buff) - 1);
                LOGD("n=%d\n", n);
                if(n == 0){
                    del_event(epfd, fd);
                    close(fd);
                    continue;
                }

                if(n == -1){
                    if(errno == EAGAIN){
                        // todo nonblocking read
                    }else{
                        del_event(epfd, fd);
                        close(fd);
                        LOGE("%s. errno = %d\n", strerror(errno), errno);
                    }
                    continue;
                }
                const char* tmp = inet_ntop(AF_INET, (void*)&client.sin_addr, ipstr, INET_ADDRSTRLEN);
                if(tmp == NULL){
                    LOGE("%s. errno = %d\n", strerror(errno), errno);
                    continue;
                }
                buff[n] = '\0';
                LOGD("from %s:%u--->%s\n", ipstr, ntohs(client.sin_port), buff);
                n = write(fd, buff, n);
                if(n == -1){
                    LOGE("%s. errno = %d\n", strerror(errno), errno);
                    del_event(epfd, fd);
                    close(fd);
                    continue;
                }
                LOGD("to %s:%u<---%s\n", ipstr,ntohs(client.sin_port), buff);
            }
        }
    }
}

int main(int argc, char *argv[]){
    int listenfd = server_init(PORT);
    if(listenfd == -1){
        return -1;
    }
    server_loop(listenfd);
    return -1;
}