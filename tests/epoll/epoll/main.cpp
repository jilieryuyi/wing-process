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
//  @link https://www.freebsd.org/cgi/man.cgi?query=kqueue&sektion=2
//  @link http://www.cnblogs.com/moonz-wu/p/4740908.html
//  @link http://www.cnblogs.com/promise6522/archive/2012/03/03/2377935.html
//

#include <iostream>
#include "queue.hpp"
#include "assert.h"
#include "wing.h"
#include "wing.hpp"
#include "time.hpp"

const int kReadEvent  = 1;
const int kWriteEvent = 2;
int listenfd;


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
void update_events(int efd, client_node* client, int events, bool modify)
{
    if (!client) {
        return;
    }
    if (client->fd <= 0) {
        return;
    }
    struct kevent ev[2];
    int n = 0;

    //添加read事件
    if (events & kReadEvent) {
        EV_SET(&ev[n++], client->fd, EVFILT_READ, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)client);
    } else if (modify) {
        EV_SET(&ev[n++], client->fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)client);
    }

    //添加write事件
    if (events & kWriteEvent) {
        EV_SET(&ev[n++], client->fd, EVFILT_WRITE, EV_ADD|EV_ERROR|EV_ENABLE, 0, 0, (void*)client);
    } else if (modify) {
        EV_SET(&ev[n++], client->fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)client);
    }
    debug("%s fd %d events read %d write %d\n",
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
        
        if (cfd < 0) {
            debug("accept failed");
            continue;
        }
        //exit_if(cfd < 0, "accept failed");
        memset(&peer, 0, alen);
        r = getpeername(cfd, (sockaddr*)&peer, &alen);
        exit_if(r < 0, "getpeername failed");
        debug("accept a connection from %s:%d\n", inet_ntoa(raddr.sin_addr), raddr.sin_port);
    
        //屏蔽sigpipe
        const int value = 1;
        setsockopt(cfd, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(int));
        set_keepalive(cfd);
        
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
    //如果可读字节数等于0，说明对端关闭了，这个时候可以释放连接了
    if (bytes <= 0) {
        debug("receive error fd %d closed\n", client->fd);
        free_client(client);
        return;
    }
    
    //接下来读取bytes字节的数据
    char buf[bytes + 1];
    memset(buf, 0, bytes + 1);
    ssize_t n = 0, m;
    
    while (1) {
        m = recv(client->fd, buf, bytes, 0);
        //读完或者遇到错误，直接中断
        if (m <= 0 || n >= bytes) {
            break;
        }
        n += m;
        debug("read %ld => %ld \n", bytes, n);
    }
    
    debug("read %zd bytes:\n%s  errno: %d\n\n", n, buf, errno);
    
    //返回0或者EPIPE和ECONNRESET均代表客户端已经断开连接了
    //EPIPE		  32 Broken pipe
    //ECONNRESET  54 Connection reset by peer
    if (m == 0 || errno == EPIPE || errno == ECONNRESET) {
        debug("receive error fd %d closed\n", client->fd);
        free_client(client);
        return;
    }
    
    char *msg           = (char*)"HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nhello world";
    size_t send_size    = strlen(msg);
    ssize_t sended_size = 0;
    
    //这里的for循环仅仅是为了测试send
    //int i = 0;
    //for (i = 0; i < 1000000; i++)
    //发送回应数据
    sended_size = send_data(efd, client, msg, send_size, 0); //写出读取的数据
    
    if (sended_size == send_size) {
        //发送成功，如果是htpp协议，可以考虑在这里free_client
        //待测试
    }
    
    //只有当receive buffer为空时，blocking模式才会等待，而nonblock模式下会立即返回-1（errno = EAGAIN或EWOULDBLOCK
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return;
    }
    
    //ENOTSOCK	38 Socket operation on non-socket
    //这个错误一般由于socket被close释放掉了，然后还进行read操作造成的
    if (errno == ENOTSOCK) {
        return;
    }
    
    exit_if(n < 0, "read error"); //实际应用中，n<0应当检查各类错误，如EINTR
}

/**
 * 可写事件回调
 * @param efd kqueue
 * @param client client_node
 * @param left_size 剩余的发送缓冲区大小
 */
void handle_write(int efd, client_node* client, size_t left_size)
{
    //缓冲区满，不可写
    if (left_size <= 0) {
        return;
    }
    
    /*
    //这里的预期是想要判断可写的缓冲区大小是否足够容纳接下来要发送的数据
    node* n = peek_queue(client->queue);
    if (n != NULL) {
        send_queue_node* sn = (send_queue_node*)n->data;
        if (sn->msg_size > left_size) {}
    }*/
    
    //发送队列pop数据
    node* n = pop_queue(client->queue);
    if (n == NULL) {
        goto end;
    }
    
    //如果存在未发送完成的数据，尝试重发
    while (n != NULL) {
        //send_queue_node* sn = (send_queue_node*)n->data;
        debug("队列发送\n");
        send_node(efd, client, n);
        n = pop_queue(client->queue);
    }
    
    if (1) {
        //如果是htpp协议，可以考虑在这里free_client
        //待测试
    }
    
end:
    update_events(efd, client, kReadEvent, true);
}

static size_t _send(int fd, const char* buf, size_t buf_size)
{
    ssize_t sended = 0;
    ssize_t size   = 0;
    char *sbuf     = (char*)buf;
    
    //nonblock write则是返回能够放下的字节数，之后调用则返回-1（errno = EAGAIN或EWOULDBLOCK）
    while (sended < buf_size) {
        //size = send(client->fd, sbuf, buf_size, MSG_NOSIGNAL);
        //mac没有这个参数MSG_NOSIGNAL,这个MSG_NOSIGNAL参数是为了解决内核抛出sigpipe信号异常的
        size = write(fd, sbuf, buf_size);
        debug("%d send msg %zu bytes:\n%s\n\n", errno, buf_size, sbuf);
        
        //如果出现EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
        if (size < 0 && errno == EINTR) {
            debug("系统中断\n");
            continue;
        }
        
        //缓冲区满
        //EAGAIN 35 Resource temporarily unavailable
        //EAGAIN 的意思也很明显，就是要你再次尝试。
        if (size < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            debug("缓冲区满\n");
            continue;
        }
        
        //EPIPE 32 对端关闭
        //ECONNRESET 54	Connection reset by peer
        if (size < 0) {
            break;
        }
        sended += size;
        sbuf = (char*)(buf + sended);
        
    }
    
    return sended;
}

static void _push_to_queue(queue* q, const char* buf, size_t buf_size, int send_times)
{
    //创建一个数据节点
    send_queue_node* _node = create_send_queue_node((char*)buf, buf_size, send_times + 1);

    node* n = create_node(q, (void*)_node);
    if (n != NULL) {
        push_queue(q, n);
    } else {
        free_send_queue_node((void*)_node);
    }
    
}

/**
 * 如果失败会自动进入重试队列，待客户端可写，重试发送，最多重试100次
 */
ssize_t send_data(int efd, client_node* client, const char* buf, size_t buf_size, int send_times)
{
    if (send_times > 5) {
        debug("resend max times error close %d\n", client->fd);
        free_client(client);
        return 0;
    }
    
    size_t size = _send(client->fd, buf, buf_size);
    
    if (size == -1 && errno != EPIPE && errno != ECONNRESET) {
        //发送失败，直接进入重试队列
        _push_to_queue(client->queue, buf, buf_size, send_times);
    }
    
    update_events(efd, client, kWriteEvent, false);
    
    //EPIPE 32 d对端关闭
    if (errno == EPIPE || errno == ECONNRESET) {
        free_client(client);
    }
    return size;
}

ssize_t send_node(int efd, client_node* client, node* n)
{
    send_queue_node* sn = (send_queue_node*)n->data;
    
    if (sn->send_times > 5) {
        free_node(n, free_send_queue_node);
        debug("resend max times error close %d\n", client->fd);
        free_client(client);
        return 0;
    }
    
    sn->send_times = sn->send_times+1;
    size_t size = _send(client->fd, sn->msg, sn->msg_size);
    
    if (size == -1 && errno != EPIPE && errno != ECONNRESET) {
        //发送失败，直接进入重试队列
        push_queue(client->queue, n);
    } else {
        free_node(n, free_send_queue_node);
    }
    
    update_events(efd, client, kWriteEvent, false);
    
    //EPIPE 32 d对端关闭
    if (errno == EPIPE || errno == ECONNRESET) {
        free_client(client);
        errno  = 0;
    }
    return size;
}

/**
 * 事件轮询
 * @param efd kqueue
 * @param server server节点
 * @param waitms 超时时间
 */
void loop(int efd, client_node* server, int waitms)
{
    struct timespec timeout;
    const int max_enents = 20;

    timeout.tv_sec  = waitms / 1000;
    timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;

    struct kevent active_events[max_enents];
    //获取ready的fd，类似epoll_wait
    int n = kevent(efd, NULL, 0, active_events, max_enents, &timeout);
    //debug("epoll_wait return %d", n);

    for (int i = 0; i < n; i ++) {
        //用户数据
        client_node* client = (client_node*)active_events[i].udata;
        //这里的data在不同的事件里面有不同的意义，accept里面代表等待连接的客户端
        //read事件里面代表可读的数据字节数
        long data      = active_events[i].data;
        uint16_t flags = active_events[i].flags;
        int events     = active_events[i].filter;
        
        debug("events = %d flags = %d data = %ld ", events, flags, data);

        if (events == EVFILT_READ) {
            //如果触发的socket等于监听的socket，说明有新的连接
            if (client->fd == server->fd) {
                handle_accept(efd, server->fd, data);
            } else {
                handle_read(efd, client, data);
            }
        }
        
        else if (events == EVFILT_WRITE) {
            handle_write(efd, client, data);
        }
        
        else {
            debug("unknown event %d\r\n", events);
            goto error;
            break;
        }
        
        //处理出错的socket
        if (flags & EV_ERROR) {
            debug("ev error close %d\n", client->fd);
            free_client(client);
            continue;
        }
    }
    
    return;
    
error:
    debug("server error close %d\n", server->fd);
    free_client(server);
    exit_if(1, "unknown event");
}


void sig_handle(int sig)
{
    debug("%d sig received ", sig);
    close(listenfd);
    exit(0);
}

int main()
{
    debug("server start ......");
    //queue_test();
    //return 0;
    
    //time_test();
    //return 0;
    
    short port  = 9998;
    int epollfd = kqueue();
    exit_if(epollfd < 0, "epoll_create failed");
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
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
    debug("fd %d listening at %d", listenfd, port);
    
    client_node* server = init_client(listenfd, (char*)"0.0.0.0", port);
    
    //屏蔽sigpipe
    const int value = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(int));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    set_keepalive(listenfd);

    set_non_block(listenfd);
    update_events(epollfd, server, kReadEvent, false);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_handle);
    signal(SIGQUIT, sig_handle);
    signal(SIGHUP, sig_handle);
    signal(SIGTERM, sig_handle);
    
    for (;;) { //实际应用应当注册信号处理函数，退出时清理资源
        loop(epollfd, server, 1000);
    }
    
    return 0;
}

