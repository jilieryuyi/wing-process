/**
 *@消息队列api
 */
#include "wing_message_queue.h"

//----消息队列----------------------------
CRITICAL_SECTION queue_lock;


/**
 *@初始化消息队列
 */
void message_queue_init( iocp_message_queue *&message_queue )  
{  
	InitializeCriticalSection(&queue_lock);
    message_queue->head = NULL; //队头标志位  
    message_queue->tail = NULL; //队尾标志位  
}  
   

/**
 *@往消息队列追加消息
 */
BOOL message_queue_lpush(iocp_message_queue *&message_queue , iocp_message_queue_element *element)  
{  
	if( message_queue == NULL || NULL == element)
		return 0;

	EnterCriticalSection(&queue_lock);

    iocp_message_queue_node *nnode = new iocp_message_queue_node();
	
    if (nnode == NULL )  
    {  
		LeaveCriticalSection(&queue_lock);
        return 0;
    } 

    nnode->data = element;  
    nnode->next = NULL;  
    if (message_queue->head == NULL)  
    {  
        message_queue->head = nnode;  
        message_queue->tail = nnode;  
    } else {  
        message_queue->tail->next = nnode;  
        message_queue->tail = nnode;  
    }  
	LeaveCriticalSection(&queue_lock);
    return 1;  
}  
  
/**
 *@弹出消息
 */
void message_queue_pop(iocp_message_queue *&message_queue ,iocp_message_queue_element *&temp)  
{  
	if( NULL == message_queue ) 
	{
		temp = NULL;
		return;
	}

	EnterCriticalSection(&queue_lock);

    if ( message_queue->head == NULL )  
    {  
		temp = NULL;
		LeaveCriticalSection(&queue_lock);
		return;
    }  

	iocp_message_queue_node * p = NULL;  
    temp = message_queue->head->data;  
    p    = message_queue->head;  
    message_queue->head = message_queue->head->next;  
    if( message_queue->head == NULL )  
    {  
        message_queue->tail = NULL;  
    }  

    delete p;  
	p  = NULL;
	LeaveCriticalSection(&queue_lock);
}  
  
/**
 *@返回消息节点 不弹出
 */
iocp_message_queue_element *peek_message_queue( iocp_message_queue *&message_queue )  
{  
	if( NULL == message_queue ) 
		return NULL;

    if ( message_queue->head == NULL)  
    {  
        return NULL; 
    }   
    return message_queue->head->data;  
}  
  
/**
 *@判断队列是否为空
 */
BOOL message_queue_isempty(iocp_message_queue *&message_queue)  
{  
	if( NULL == message_queue ) return 1;
    return message_queue->head == NULL;  
}  
 
/**
 *@清空队列
 */
void message_queue_clear(iocp_message_queue *&message_queue)  
{  
	if( NULL == message_queue ) return;

    iocp_message_queue_node *p = message_queue->head;  
    while(p != NULL)  
    {  
        message_queue->head = message_queue->head->next;  
        delete p; 
		p = NULL;
        p = message_queue->head;  
    }  
    message_queue->tail = NULL;  
	DeleteCriticalSection(&queue_lock);
	delete message_queue;
	message_queue = NULL;
    return;  
}  
//----消息队列----------end------------------