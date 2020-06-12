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
#include <unistd.h>
#include <fcntl.h>
void tcp_server(uint16_t port){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        LOGE("error:%s, errno=%d\n", strerror(errno), errno);
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
        printf("client in\n");
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
    int conn, n;
    if(add_event(epfd, listenfd) != 0){
        return;
    }
    struct epoll_event events[EVENTS_MAX];
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    while(1){
        n = epoll_wait(epfd, events, EVENTS_MAX, -1);
        if(n == -1){
            LOGE("%s, errno=%d\n", strerror(errno), errno);
            LOGD("server exit\n");
            exit(1);
        }
        for (int i = 0; i < n; i++){
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
            }
        }
    }
}

int main(int argc, char *argv[]){
    tcp_server(PORT);
    return -1;
}