#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H
#include "Windows.h"

typedef struct _iocp_message_queue_element{
	int message_id;
	unsigned long lparam;
	unsigned long wparam;
	unsigned long eparam;

} iocp_message_queue_element;

typedef struct _iocp_message_queue_node   
{  
    iocp_message_queue_element *data;  
    struct _iocp_message_queue_node * next;  
} iocp_message_queue_node;            // 节点的结构  
  
typedef struct _iocp_message_queue  
{  
    iocp_message_queue_node * head;  
    iocp_message_queue_node * tail;  
} iocp_message_queue;          // 队列的结构  





/**
 *@初始化消息队列
 */
void message_queue_init( iocp_message_queue* &message_queue ); 

/**
 *@往消息队列追加消息
 */
BOOL message_queue_lpush(iocp_message_queue *&message_queue , iocp_message_queue_element *element);  
  
/**
 *@弹出消息
 */
void message_queue_pop(iocp_message_queue *&message_queue ,iocp_message_queue_element *&temp);  
  
/**
 *@返回消息节点 不弹出
 */
iocp_message_queue_element *peek_message_queue( iocp_message_queue *&message_queue );  
  
/**
 *@判断队列是否为空
 */
BOOL message_queue_isempty(iocp_message_queue *&message_queue);
 
/**
 *@清空队列
 */
void message_queue_clear(iocp_message_queue *&message_queue);


#endif