//
//  wing.cpp
//  epoll
//
//  Created by yuyi on 17/10/28.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <stdio.h>
#include "wing.h"
#include "wing.hpp"

/**
 * 初始化客户端节点资源
 * @param fd socket句柄
 * @param ip 连接ip
 * @param port 连接端口
 * @return client_node*
 */
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
    client->last_revc_time = 0;
    client->last_send_time = 0;
    
    return client;
}

/**
 * 释放客户端
 */
void free_client(client_node* client)
{
    if (client == NULL) {
        return;
    }
    shutdown(client->fd, SHUT_RDWR);
    close(client->fd);
    client->fd = 0;
    free_queue(client->queue, free_send_queue_node);
    client->queue = NULL;
    
    if (client) {
        free(client);
    }
    client = NULL;
}


/**
 * 创建一个发送队列节点数据，2个malloc
 */
send_queue_node* create_send_queue_node(char* buf, size_t buf_size, int send_times)
{
    send_queue_node* node = (send_queue_node*)malloc(sizeof(send_queue_node));
    node->send_times      = send_times + 1;
    node->msg             = (char*)malloc(buf_size+1);
    
    memset(node->msg, 0, buf_size + 1);
    sprintf(node->msg, buf, buf_size);
    node->msg_size = buf_size;
    
    return node;
}


/**
 * 释放发送的队列数据，2个free
 */
void free_send_queue_node(void* n)
{
    send_queue_node* sn = (send_queue_node*)n;
    if(sn->msg) {
        free(sn->msg);
        sn->msg = NULL;
    }
    free(n);
    n = NULL;
}


/**
 * 设置socket的keepalive
 * @param sockfd socket句柄
 */
void set_keepalive(int sockfd)
{
    const uint32_t keepaliveIntervalSec = 10;
    
#ifdef _WIN32
    tcp_keepalive keepaliveParams;
    DWORD ret = 0;
    keepaliveParams.onoff = 1;
    keepaliveParams.keepaliveinterval = keepaliveParams.keepalivetime = keepaliveIntervalSec * 1000;
    WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, &keepaliveParams, sizeof(keepaliveParams), NULL, 0, &ret, NULL, NULL);
    
#elif __APPLE__
    int on = 1, secs = keepaliveIntervalSec;
    setsockopt(sockfd, SOL_SOCKET,  SO_KEEPALIVE, &on, sizeof(on));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE, &secs, sizeof(secs));
    
#elif __linux
    int32_t optval;
    socklen_t optlen = sizeof(optval);
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    const uint32_t keepaliveProbeCount = 20;
    optval = keepaliveIntervalSec;
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);
    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, &optval, optlen);
    optval = keepaliveProbeCount;
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, &optval, optlen);
#endif
}
