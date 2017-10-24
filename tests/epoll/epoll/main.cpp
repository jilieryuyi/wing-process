//
//  main.cpp
//  epoll
//
//  Created by yuyi on 17/10/22.
//  Copyright © 2017年 yuyi. All rights reserved.
//
//  kqueue demo
//  @link http://blog.csdn.net/bytxl/article/details/17526351
//  @link https://www.ibm.com/developerworks/cn/aix/library/1105_huangrg_kqueue/
//

#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "stdlib.h"
#include "queue.hpp"
#include "assert.h"
#include "wing.h"

const int kReadEvent  = 1;
const int kWriteEvent = 2;

char* pop_queue();

/**
 * 将socket设置为非阻塞
 * @params fd socket资源句柄
 */
void set_non_block(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    exit_if(flags < 0, "fcntl failed");
    
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    exit_if(r < 0, "fcntl failed");
}

/**
 * 添加或删除read、write监听
 * @param efd kqueue返回值
 * @param fd socket句柄
 * @param events 操作类型
 * @param modify 是否修改
 */
void update_events(int efd, int fd, int events, bool modify)
{
    struct kevent ev[2];
    int n = 0;

    /**
     #define EV_SET(kevp, a, b, c, d, e, f) do {	\
     struct kevent *__kevp__ = (kevp);	\
     __kevp__->ident = (a);			\
     __kevp__->filter = (b);			\
     __kevp__->flags = (c);			\
     __kevp__->fflags = (d);			\
     __kevp__->data = (e);			\
     __kevp__->udata = (f);			\
     } while(0)
     */
    //添加read事件
    if (events & kReadEvent) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }

    //添加write事件
    if (events & kWriteEvent) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    printf("%s fd %d events read %d write %d\n",
           modify ? "mod" : "add", fd, events & kReadEvent, events & kWriteEvent);

    //使read、write事件生效
    int r = kevent(efd, ev, n, NULL, 0, NULL);
    exit_if(r, "kevent failed ");
}

/**
 * accept接受连接
 * @param efd kqueue
 * @param fd listen的socket句柄
 * @param nums 新连接的个数
 */
void handle_accept(int efd, int fd, long nums)
{
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd, i, r;
    sockaddr_in peer;//, local;
    socklen_t alen = sizeof(peer);
    
    for (i = 0; i < nums; i++) {
        memset(&raddr, 0, rsz);
        cfd = accept(fd,(struct sockaddr *)&raddr, &rsz);
        exit_if(cfd < 0, "accept failed");
    
        memset(&peer, 0, alen);
        r = getpeername(cfd, (sockaddr*)&peer, &alen);
        exit_if(r < 0, "getpeername failed");
        printf("accept a connection from %s:%d\n", inet_ntoa(raddr.sin_addr), raddr.sin_port);
    
        set_non_block(cfd);
        update_events(efd, cfd, kReadEvent|kWriteEvent, false);
    }
}

/**
 * 读事件回调
 * @param efd kqueue
 * @param fd socket句柄
 * @param bytes 缓冲区可读字节数
 */
void handle_read(int efd, int fd, long bytes)
{
    char buf[bytes];
    ssize_t n = 0;
    
    while ((n = ::read(fd, buf, bytes)) > 0) {
        printf("read %zd bytes:\n%s\n\n", n, buf);
        ssize_t r = ::write(fd, buf, n); //写出读取的数据
        //实际应用中，写出数据可能会返回EAGAIN，此时应当监听可写事件，当可写时再把数据写出
        exit_if(r <= 0, "write error");
    }
    
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return;
    }
    exit_if(n < 0, "read error"); //实际应用中，n<0应当检查各类错误，如EINTR
    printf("fd %d closed\n", fd);
    close(fd);
}

/**
 * 写事件回调
 * @param efd kqueue
 * @param fd socket句柄
 */
void handle_write(int efd, int fd)
{
    char *send_data = pop_queue();
    //实际应用应当实现可写时写出数据，无数据可写才关闭可写事件
    if (send_data == NULL) {
        //关闭写事件
        update_events(efd, fd, kWriteEvent, true);
        return;
    }
    
    ssize_t size = send(fd, send_data, strlen(send_data), 0);
    if (size == -1) {
        //send error
    }
}

/**
 * 写入发送队列
 * @param buf 需要发送的数据
 */
void endQueue(int efd, int fd, const char *buf)
{
    //启用写事件
    update_events(efd, fd, kWriteEvent, false);
}

/**
 * 弹出队列 
 */
char* pop_queue()
{
    return NULL;
}

void sendData(int efd, int fd, const char* buf, size_t buf_size)
{
    ssize_t size = send(fd, buf, buf_size, 0);
    
    if (size == -1) {
        endQueue(efd, fd, buf);
        return;
    }
    
    if (size < buf_size) {
        char *_buf = (char*)(buf+size);
        endQueue(efd, fd, _buf);
        return;
    }
}

/**
 * 事件轮训
 * @param efd kqueue
 * @param lfd 监听 socket
 * @param waitms 超时时间
 */
void loop(int efd, int lfd, int waitms)
{
    struct timespec timeout;
    const int kMaxEvents = 20;

    timeout.tv_sec  = waitms / 1000;
    timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;

    struct kevent activeEvs[kMaxEvents];
    //获取ready的fd，类似epoll_wait
    int n = kevent(efd, NULL, 0, activeEvs, kMaxEvents, &timeout);
    printf("epoll_wait return %d\n", n);

    for (int i = 0; i < n; i ++) {
        int fd = (int)(intptr_t)activeEvs[i].udata;
        long data = activeEvs[i].data;
        
        //处理出错的socket
        if (activeEvs[i].flags & EV_ERROR) {
            close((int)activeEvs[i].ident);
            continue;
        }
        
        int events = activeEvs[i].filter;
        
        if (events == EVFILT_READ) {
            //如果触发的socket等于监听的socket，说明有新的连接
            if (fd == lfd) {
                handle_accept(efd, fd, data);
            } else {
                handle_read(efd, fd, data);
            }
        } else if (events == EVFILT_WRITE) {
            handle_write(efd, fd);
        } else {
            printf("unknown event %d\r\n", events);
            goto error;
            break;
        }
    }
    
    return;
    
error:
    close(lfd);
    exit_if(1, "unknown event");
}


//释放data的函数，用于释放队列以及其节点的函数指针
void free_data(void* data)
{
    if (data) {
        free(data);
    }
    data = NULL;
}

int queue_test()
{
    queue* q = create_queue();
    mem_block *block = create_mem_block(10240);
    
    char* data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello1");
    
    node* n = create_node(block, data);
    push(q, n);
    
    
    data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello2");
    
    n = create_node(block, data);
    push(q, n);
    
    exit_if(q->length != 2, "队列出错");
    
    printf("queue length = %lu\n", q->length);
    node* c = q->first;
    printf("======================\n");
    while(c->next != NULL) {
        printf("%s\n", c->data);
    }
    if (c) printf("%s\n", c->data);
    printf("======================\n");
    
    node* n1 = pop_queue(block, q);
    printf("%s\n", n1->data);
    
    mem_move(block);
    
    free_queue(block, q, free_data);
    
    return 0;
}

int main()
{
    queue_test();
    return 0;
    
    short port  = 9998;
    int epollfd = kqueue();
    exit_if(epollfd < 0, "epoll_create failed");
    
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    exit_if(listenfd < 0, "socket failed");
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int r = ::bind(listenfd,(struct sockaddr *)&addr, sizeof(struct sockaddr));
    exit_if(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
    
    r = listen(listenfd, 20);
    exit_if(r, "listen failed %d %s", errno, strerror(errno));
    
    printf("fd %d listening at %d\n", listenfd, port);
    
    set_non_block(listenfd);
    update_events(epollfd, listenfd, kReadEvent, false);
    
    for (;;) { //实际应用应当注册信号处理函数，退出时清理资源
        loop(epollfd, listenfd, 10000);
    }
    
    return 0;
}

