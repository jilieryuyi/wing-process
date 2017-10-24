//
//  queue.hpp
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#ifndef queue_hpp
#define queue_hpp

#include <stdio.h>
typedef struct _node {
    void* data;
    //_node* prev;
    _node* next;
} node;

typedef struct _queue {
    _node *first;
    _node *last;
    unsigned long length;
} queue;




//创建一个队列
queue* create_queue();
//释放一个队列
void free_queue(queue* q, void (*free_data)(void*));

void push(queue* q, node *n);
node* pop(queue* q);
unsigned long length(queue* q);

node* create_node(void* data);
void free_node(node* n, void (*free_data)(void*));

#endif /* queue_hpp */
