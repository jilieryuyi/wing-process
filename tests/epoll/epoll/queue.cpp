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
mem_block *_create_mem_block(unsigned long size)
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

/**
 * 内存移位，对齐操作
 */
void _mem_move(mem_block* block)
{
    size_t _start = (size_t)block->start;
    size_t size   = sizeof(node);
    size_t end = _start + block->offset;
    
    //开始有数据的位置
    unsigned long start = block->empty_offset + _start;
    
    while (start < end) {
        memset((void*)_start, 0, size);
        memcpy((void*)_start, (void*)(start), size);

        _start += size;
        start  += size;
        block->offset -= size;
    }
    
    memset((void*)(_start+block->offset), 0, block->max_size - block->offset);
    block->empty_offset = 0;
}

void mem_move_ext(mem_block* block, size_t _start)
{
    size_t size = sizeof(node);
    size_t end  = _start + block->offset;
    
    //开始有数据的位置
    unsigned long start = _start + size;
    
    while (start < end) {
        memset((void*)_start, 0, size);
        memcpy((void*)_start, (void*)(start), size);
        
        _start += size;
        start  += size;
        block->offset -= size;
    }
    
    memset((void*)(_start+block->offset), 0, block->max_size - block->offset);
    block->empty_offset = 0;
}








/**
 * 创建一个队列
 * @return queue*
 */
queue* create_queue(size_t max_size)
{
    queue *q  = (queue*)malloc(sizeof(queue));
    q->first  = NULL;
    q->last   = NULL;
    q->length = 0;
    q->block  = _create_mem_block(max_size);
    return q;
}


void _free_queue(queue* q)
{
    free(q->block);
    q->block = NULL;
    free(q);
    q = NULL;
}


/**
 * 释放一个队列
 * @param q queue*需要释放的队列
 * @param free_data 释放节点数据的函数指针
 */
void free_queue(queue* q, void (*free_data)(void*))
{
    node* current = q->first;
    node* temp    = NULL;

    if (current) {
        while (1) {
            temp = current->next;
            free_node(current, free_data);
            q->length--;
            current = temp;
            
            if (current == NULL) {
                break;
            }
        }
    }
    _free_queue(q);
}

/**
 * 追加元素到队列尾部
 * @param q queue*需要追加元素的队列
 * @param n node*添加的节点
 */
int push_queue(queue* q, node *n)
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
node* pop_queue(queue* q)
{
    if (!q) {
        return NULL;
    }
    node *t  = q->first;
    if ( t == NULL) {
        return NULL;
    }
    q->first = t->next;
    
    if (q->first) {
        q->first->prev = NULL;
    }
    
    q->length--;
    q->block->empty_offset += sizeof(node);
    
    //触发内存移位的条件为有一半空闲了，或者已经满了
    if (q->block->empty_offset >= q->block->max_size/2) {
        _mem_move(q->block);
    }
    
    return t;
}

/**
 * 删除任意节点
 * @param q 队列
 * @param free_data 释放节点的函数指针
 */
void del_node(queue* q, node* n, void (*free_data)(void *))
{
    node* prev = n->prev;
    node* next = n->next;
    
    if (prev) {
        prev->next = next;
    }
    
    if (next) {
        next->prev = prev;
    }
    
    mem_move_ext(q->block, n->offset);
    free_node(n, free_data);
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
node* create_node(queue *q, void* data)
{
    if (q->block->offset >= q->block->max_size) {
        return NULL;
    }
    
    node* t = (node*)((unsigned long)q->block->start + q->block->offset);//malloc(sizeof(node));
    t->data = data;
    t->next = NULL;
    t->prev = NULL;
    t->offset = q->block->offset;
    q->block->offset += sizeof(node);
    
    if (q->block->offset >= q->block->max_size) {
        _mem_move(q->block);
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



