//
//  queue.cpp
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include "queue.hpp"
#include "stdlib.h"

//创建一大块内存 返回开始指针
mem_block *create_mem_block(unsigned long size)
{
    size = (int(size/sizeof(node))+1)*sizeof(node);
    
    void* start = malloc(size);
    memset(start, 0, size);
    mem_block* block = (mem_block*)malloc(sizeof(mem_block));
    memset(block, 0, sizeof(mem_block));
    block->start = start;
    block->offset = 0;
    block->empty_offset = 0;
    block->max_size = size;
    return block;
}

//内存移位
void mem_move(mem_block* block)
{
    size_t _start = (size_t)block->start;
    size_t size   = sizeof(node);
    unsigned long start = block->empty_offset;
    
    while (start < block->max_size) {
        memcpy((void*)_start, (void*)start, size);
        _start += size;
        start  += size;
    }
    
}

/**
 * 创建一个队列
 * @return queue*
 */
queue* create_queue()
{
    queue *q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    q->last  = NULL;
    q->length = 0;
    return q;
}

/**
 * 释放一个队列
 * @param q queue*需要释放的队列
 * @param free_data 释放节点数据的函数指针
 */
void free_queue(mem_block *block, queue* q, void (*free_data)(void*))
{
    node* current = q->first;
    node* temp = NULL;

    if (current) {
        while (current->next != NULL) {
            temp = current->next;
            free_node(current, free_data);
            q->length--;
            current = temp;
        }
        
        free_node(current, free_data);
        q->length--;
        current = NULL;
    }
    
    free(q);
    q = NULL;
    free(block->start);
    block->start = NULL;
    free(block);
    block = NULL;
}

/**
 * 追加元素到队列尾部
 * @param q queue*需要追加元素的队列
 * @param n node*添加的节点
 */
int push(queue* q, node *n)
{
    exit_if(!q, "队列q == NULL");
    
    if (!q) {
        return 0;
    }
    node *t = q->last;
    
    if (t == NULL) {
        q->last  = n;
        q->first = n;
        q->length++;
        return 1;
    }
    
    t->next = n;
    n->prev = t;
    q->last = n;
    q->length++;
    return 1;
}

/**
 * 从队列首部弹出一个元素
 * @param q queue*队列
 * @return node*
 */
node* pop_queue(mem_block *block, queue* q)
{
    if (!q) {
        return NULL;
    }
    node *t  = q->first;
    q->first = t->next;
    
    if (q->first) {
        q->first->prev = NULL;
    }
    
    q->length--;
    block->empty_offset += sizeof(node);
    
    //触发内存移位的条件为有一半空闲了，或者已经满了
    if (block->empty_offset >= block->max_size/2) {
        mem_move(block);
    }
    
    return t;
}

/**
 * 获取队列长度
 * @param q queue*队列
 * @return int
 */
unsigned long length(queue* q)
{
    return q->length;
}

//创建一个节点
node* create_node(mem_block *block, void* data)
{
    if (block->offset >= block->max_size) {
        return NULL;
    }
    
    node* t = (node*)((unsigned long)block->start+block->offset);//malloc(sizeof(node));
    t->data = data;
    t->next = NULL;
    t->prev = NULL;
    block->offset += sizeof(node);
    
    if (block->offset >= block->max_size) {
        mem_move(block);
    }
    
    return t;
}

//释放一个节点
void free_node(node* n, void (*free_data)(void*))
{
    if (n->data) {
        free_data(n->data);
    }
}



