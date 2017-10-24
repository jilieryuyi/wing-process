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
#include "wing.h"

typedef struct _node {
    void* data;
    _node* prev;
    _node* next;
} node;

typedef struct _queue {
    _node *first;
    _node *last;
    unsigned long length;
} queue;

typedef struct _mem_block {
    void *start;
    unsigned long empty_offset;
    unsigned long max_size;
    unsigned long offset;
} mem_block;


//创建一大块内存 返回开始指针
mem_block *create_mem_block(unsigned long size);
//内存移位
void mem_move(mem_block* mem_block);



//创建一个队列
queue* create_queue();
//释放一个队列
void free_queue(mem_block *block, queue* q, void (*free_data)(void*));
//队列尾部追加元素
int push(queue* q, node *n);
//队列顶部弹出元素
node* pop_queue(mem_block *block, queue* q);
//队列长度
unsigned long length(queue* q);

//创建一个节点
node* create_node(mem_block *block, void* data);
//释放一个节点
void free_node(node* n, void (*free_data)(void*));

#endif /* queue_hpp */
