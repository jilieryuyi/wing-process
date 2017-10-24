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

#define SEND_QUEUE_BUF_SIZE 10240
#define MAX_SEND_TIMES 100 //发送失败最大的重试次数

const int kReadEvent  = 1;
const int kWriteEvent = 2;

void free_data(void* data);


typedef struct _client {
    int fd;
    queue* queue;
    char ip[16];
    int port;
} client_node;


typedef struct _send_queue_node {
    client_node* client;
    char* _msg;
    unsigned long _msg_size;
    int send_times;
} send_queue_node;

ssize_t send_data(int efd, client_node* client, const char* buf, size_t buf_size, int send_times);
client_node* init_client(int fd, char* ip, int port)
{
    client_node *client = (client_node*)malloc(sizeof(client_node));
    memset(client, 0, sizeof(client_node));
    
    client->fd = fd;
    
    //123.123.123.123
    memset(client->ip, 0, 16);
    sprintf(client->ip, "%s", ip);
    client->port = port;
    client->queue = create_queue(SEND_QUEUE_BUF_SIZE);
    
    return client;
}

void free_send_queue_node(void* n);
/**
 * 释放客户端
 */
void free_client(client_node* client)
{
    close(client->fd);
    client->fd = 0;
    free_queue(client->queue, free_send_queue_node);
    client->queue = NULL;
    free(client);
    client = NULL;
}

/**
 * 释放发送的队列数据
 */
void free_send_queue_node(void* n)
{
    send_queue_node* _n = (send_queue_node*)n;
    if(_n->_msg) {
        free(_n->_msg);
    }
    free(n);
    n = NULL;
}

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
 * @param client client节点指针
 * @param events 操作类型
 * @param modify 是否修改
 */
void update_events(int efd, /*int fd*/client_node* client, int events, bool modify)
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
        EV_SET(&ev[n++], client->fd, EVFILT_READ, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)client);//(intptr_t)fd);
    } else if (modify) {
        EV_SET(&ev[n++], client->fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)client);//(intptr_t)fd);
    }

    //添加write事件
    if (events & kWriteEvent) {
        EV_SET(&ev[n++], client->fd, EVFILT_WRITE, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)client);//(intptr_t)fd);
    } else if (modify) {
        EV_SET(&ev[n++], client->fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)client);//(intptr_t)fd);
    }
    printf("%s fd %d events read %d write %d\n",
           modify ? "mod" : "add", client->fd, events & kReadEvent, events & kWriteEvent);

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
    
        client_node *client = init_client(cfd, inet_ntoa(raddr.sin_addr), raddr.sin_port);
        
        set_non_block(cfd);
        update_events(efd, client, kReadEvent|kWriteEvent, false);
    }
}

/**
 * 读事件回调
 * @param efd kqueue
 * @param client client_node
 * @param bytes 缓冲区可读字节数
 */
void handle_read(int efd, client_node* client, long bytes)
{
    char buf[bytes + 1];
    memset(buf, 0, bytes + 1);
    ssize_t n = 0;
    
    while (1) {
        n += ::read(client->fd, buf, bytes);
        if (n < 0 || n >= bytes) {
            break;
        }
    }
    
    printf("read %zd bytes:\n%s\n\n", n, buf);
    char *msg = (char*)"HTTP/1.1 200 OK\r\n\r\nhello world";
    size_t send_size = strlen(msg);
    ssize_t sended_size = send_data(efd, client, msg, send_size, 0); //写出读取的数据
    //free_client(client);
    
    if (sended_size == send_size) {
        //发送成功，如果是htpp协议，可以考虑在这里free_client
    }
    
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return;
    }
    exit_if(n < 0, "read error"); //实际应用中，n<0应当检查各类错误，如EINTR
    printf("fd %d closed\n", client->fd);
    free_client(client);//(fd);
}

/**
 * 写事件回调
 * @param efd kqueue
 * @param client client_node
 */
void handle_write(int efd, client_node* client)
{
    node* n = pop_queue(client->queue);
    if (n == NULL) {
        update_events(efd, client, kReadEvent, true);
        return;
    }
    
    while (n != NULL) {
        send_queue_node* sn = (send_queue_node*)n->data;
        send_data(efd, client, (const char*)sn->_msg, (size_t)sn->_msg_size, sn->send_times);
        n = pop_queue(client->queue);
    }
    
    if (1) {
        //如果是htpp协议，可以考虑在这里free_client
    }
    
    update_events(efd, client, kReadEvent, true);
}

/**
 * 如果失败会自动进入重试队列，待客户端可写，重试发送，最多重试100次
 */
ssize_t send_data(int efd, client_node* client, const char* buf, size_t buf_size, int send_times)
{
    if (send_times > MAX_SEND_TIMES) {
        return 0;
    }
    
    ssize_t size = send(client->fd, buf, buf_size, 0);
    
    //创建一个数据节点
    send_queue_node* _node = (send_queue_node*)malloc(sizeof(send_queue_node));
    _node->send_times = send_times + 1;
    _node->_msg = (char*)malloc(buf_size+1);
    memset(_node->_msg, 0, buf_size + 1);
    
    //创建一个队列节点，用于容纳数据节点
    node* n = create_node(client->queue, (void*)_node);
    
    if (size == -1) {
        sprintf(_node->_msg, buf, buf_size);
        _node->_msg_size = buf_size;
        push_queue(client->queue, n);
        update_events(efd, client, kWriteEvent, false);
        return size;
    }
    
    if (size < buf_size) {
        char *_buf = (char*)(buf+size);
        sprintf(_node->_msg, _buf, buf_size - size);
         _node->_msg_size = buf_size - size;
        push_queue(client->queue, n);
        update_events(efd, client, kWriteEvent, false);
        return size;
    }
    
    return size;
}

/**
 * 事件轮训
 * @param efd kqueue
 * @param server server节点
 * @param waitms 超时时间
 */
void loop(int efd, client_node* server, int waitms)
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
        client_node* client = (client_node*)activeEvs[i].udata;
        long data = activeEvs[i].data;
        
        //处理出错的socket
        if (activeEvs[i].flags & EV_ERROR) {
            close((int)activeEvs[i].ident);
            continue;
        }
        
        int events = activeEvs[i].filter;
        
        if (events == EVFILT_READ) {
            //如果触发的socket等于监听的socket，说明有新的连接
            if (client->fd == server->fd) {
                handle_accept(efd, server->fd, data);
            } else {
                handle_read(efd, client, data);
            }
        } else if (events == EVFILT_WRITE) {
            handle_write(efd, client);
        } else {
            printf("unknown event %d\r\n", events);
            goto error;
            break;
        }
    }
    
    return;
    
error:
    free_client(server);
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


/**
 * 队列测试
 */
int queue_test()
{
    //创建一个队列
    queue* q = create_queue(SEND_QUEUE_BUF_SIZE);
    
    //源数据
    char* data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello1");
    
    //创建一个节点，这个节点的内存直接来自于上面的内存块
    node* n = create_node(q, data);
    //push到队列尾部
    push_queue(q, n);
    
    //第二个源数据
    data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello2");
    
    //创建第二个节点
    n = create_node(q, data);
    //将第二个节点push到队列尾部
    push_queue(q, n);
    
    exit_if(q->length != 2, "队列出错");
    
    printf("queue length = %lu\n", q->length);
    
    //队列遍历操作
    node* c = q->first;
    printf("======================\n");
    while(c->next != NULL) {
        printf("%s\n", c->data);
        c = c->next;
    }
    if (c) {
        printf("%s\n", c->data);
    }
    //遍历操作-end
    
    printf("======================\n");
    
    printf("offset = %lu\n", q->block->offset);
    
    //pop一个元素
    node* n1 = pop_queue(q);
    printf("%s\n", n1->data);
    printf("%lu\n", q->block->empty_offset);
    
    //执行内存对齐操作
    _mem_move(q->block);
    
    //确认对齐结果
    node* n2 = (node*)q->block->start;
    printf("%ld ==> %s\n", q->block->offset, n2->data);
    printf("%lu\n", q->block->empty_offset);
    
    //释放队列和内存块
    free_queue(q, free_data);
    
    return 0;
}

int main()
{
    //queue_test();
    //return 0;
    
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
    
    client_node* server = init_client(listenfd, (char*)"0.0.0.0", port);
    
    set_non_block(listenfd);
    update_events(epollfd, server, kReadEvent, false);
    
    for (;;) { //实际应用应当注册信号处理函数，退出时清理资源
        loop(epollfd, server, 10000);
    }
    
    return 0;
}

