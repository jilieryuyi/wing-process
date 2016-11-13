#ifndef _IOCP_MESSAGE_QUEUE_H
#define _IOCP_MESSAGE_QUEUE_H
#include "Windows.h"
/***
 *@iocp消息队列初始化
 */
void iocp_message_queue_init();
void iocp_message_queue_get(iocp_message_queue_element *&msg);
void iocp_post_queue_msg( int message_id,unsigned long wparam=0,unsigned long lparam=0,unsigned long eparam=0 );
void iocp_message_queue_clear();
#endif