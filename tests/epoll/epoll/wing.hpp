//
//  wing.hpp
//  epoll
//
//  Created by yuyi on 17/10/28.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#ifndef wing_hpp
#define wing_hpp

#include <errno.h>
#include <string.h>
#include "time.hpp"
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
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "queue.hpp"

#define	TCP_NODELAY             0x01    /* don't delay send to coalesce packets */
#define	TCP_MAXSEG              0x02    /* set maximum segment size */
#define TCP_NOPUSH              0x04    /* don't push last block of write */
#define TCP_NOOPT               0x08    /* don't use TCP options */
#define TCP_KEEPALIVE           0x10    /* idle time used when SO_KEEPALIVE is enabled */
#define TCP_CONNECTIONTIMEOUT   0x20    /* connection timeout */
#define PERSIST_TIMEOUT		    0x40	/* time after which a connection in
* persist timeout will terminate.
* see draft-ananth-tcpm-persist-02.txt
*/
#define TCP_RXT_CONNDROPTIME    0x80	/* time after which tcp retransmissions will be
* stopped and the connection will be dropped
*/
#define TCP_RXT_FINDROP		    0x100	/* when this option is set, drop a connection
* after retransmitting the FIN 3 times. It will
* prevent holding too many mbufs in socket
* buffer queues.
*/
#define	TCP_KEEPINTVL		    0x101	/* interval between keepalives */
#define	TCP_KEEPCNT		        0x102	/* number of keepalives before close */
#define	TCP_SENDMOREACKS	    0x103	/* always ack every other packet */
#define	TCP_ENABLE_ECN		    0x104	/* Enable ECN on a connection */
#define	TCP_FASTOPEN		    0x105	/* Enable/Disable TCP Fastopen on this socket */
#define	TCP_CONNECTION_INFO	    0x106	/* State of TCP connection */


#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif

//客户端节点
typedef struct _client {
    int fd;             //socket句柄
    queue* queue;       //队列指针
    char ip[16];        //客户端ip
    int port;           //客户端连接端口
    int last_send_time; //最后的发送时间
    int last_revc_time; //最后的读取时间
} client_node;

//发送队列节点数据
typedef struct _send_queue_node {
    client_node* client;    //客户端节点
    char* msg;              //待发送的数据
    unsigned long msg_size; //待发送的数据大小
    int send_times;         //发送计数器，当达到MAX_SEND_TIMES时直接放弃重试
} send_queue_node;

//发送数据，自带重试机制
ssize_t send_data(int efd, client_node* client, const char* buf, size_t buf_size, int send_times);
//释放发送队列节点数据
void free_send_queue_node(void* n);
//发送一个队列节点，一般用于失败重试
ssize_t send_node(int efd, client_node* client, node* n);
//启用和设置socket的keepalive模式
void set_keepalive(int sockfd);
client_node* init_client(int fd, char* ip, int port);
void free_client(client_node* client);
send_queue_node* create_send_queue_node(char* buf, size_t buf_size, int send_times);
void free_send_queue_node(void* n);
void set_keepalive(int sockfd);
#endif /* wing_h */
