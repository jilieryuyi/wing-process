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

//队列节点
typedef struct _node {
    void* data;
    _node* prev;
    _node* next;
    unsigned long offset;
} node;

//队列节点内存池
typedef struct _mem_block {
    void *start;
    unsigned long empty_offset;
    unsigned long max_size;
    unsigned long offset;
} mem_block;

//队列
typedef struct _queue {
    _node *first;
    _node *last;
    _mem_block* block;
    unsigned long length;
} queue;


//创建一大块内存 返回开始指针
//mem_block *create_mem_block(unsigned long size);
//内存移位
void _mem_move(mem_block* mem_block);

//创建一个队列
queue* create_queue(size_t max_size);

//释放一个队列
void free_queue(queue* q, void (*free_data)(void*));

//队列尾部追加元素
int push_queue(queue* q, node *n);

//队列顶部弹出元素
node* pop_queue(queue* q);

//队列长度
unsigned long length(queue* q);

//删除节点
void del_node(queue* q, node* n, void (*free_data)(void *));

//创建一个节点
node* create_node(queue* q, void* data);

//释放一个节点
void free_node(node* n, void (*free_data)(void*));

#endif /* queue_hpp */
