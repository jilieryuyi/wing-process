//
//  queue.cpp
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include "queue.hpp"
#include "stdlib.h"
#include "string.h"

/**
 * 创建内存池，返回开始指针，这里2次malloc
 */
mem_block *_create_mem_block(unsigned long size)
{
    if (size <= 0) {
        return NULL;
    }
    size = (int(size/sizeof(node))+1) * sizeof(node);
    
    void* start = malloc(size);
    memset(start, 0, size);
    
    mem_block* block = (mem_block*)malloc(sizeof(mem_block));
    if (block == NULL) {
        return NULL;
    }
    memset(block, 0, sizeof(mem_block));
    
    block->start        = start;
    block->offset       = 0;
    block->empty_offset = 0;
    block->max_size     = size;
    
    return block;
}

/**
 * 释放内存池，2次free，对应创建内存池的2次malloc
 */
void _free_mem_block(mem_block* block)
{
    if (!block) {
        return;
    }
    if (block->start) {
        free(block->start);
        block->start = NULL;
    }
    free(block);
    block = NULL;
}

/**
 * 内存移位，对齐操作
 */
void _mem_move(mem_block* block)
{
    if (block->empty_offset <= 0) {
        //不需要进行内存对齐操作
        return;
    }
    
    size_t _start = (size_t)block->start;
    size_t size   = sizeof(node);
    size_t end    = _start + block->offset;
    
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


/**
 * 暂时没用到
 */
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
 * 创建一个队列，1次malloc
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

/**
 * 释放队列，1次free
 */
void _free_queue(queue* q)
{
    if (!q) {
        return;
    }
    _free_mem_block(q->block);
    q->block = NULL;
    free(q);
    q = NULL;
}


/**
 * 释放一个队列，释放所有的节点，最后释放队列
 * @param q queue*需要释放的队列
 * @param free_data 释放节点数据的函数指针
 */
void free_queue(queue* q, void (*free_data)(void*))
{
    if (!q) {
        return;
    }
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
    if (!q || !n) {
        return 0;
    }
    
    printf("push queue ========== %lu\n", (q->length+1));
    
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

node* peek_queue(queue* q)
{
    if (q == NULL) {
        return NULL;
    }
    
    return q->first;
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
    if (t == NULL) {
        return NULL;
    }
    
    q->first = t->next;
    
    if (q->first) {
        q->first->prev = NULL;
    }
    
    q->length--;
    q->block->empty_offset += sizeof(node);
    
    //触发内存移位的条件为有一半空闲了
    if (q->block->empty_offset >= q->block->max_size/2) {
        _mem_move(q->block);
    }
    
    //如果内存满，pop掉之后立马进行内存对齐，这样就可以有剩余的内存了
    if (q->block->offset >= q->block->max_size) {
        _mem_move(q->block);
    }
    
    return t;
}

/**
 * 删除任意节点，暂时没有用到
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

/**
 * 创建一个节点
 */
node* create_node(queue *q, void* data)
{
    if (!q) {
        return NULL;
    }
    
    if (!q->block) {
        return NULL;
    }
    
    if (q->block->offset >= q->block->max_size) {
        //内存池满
        return NULL;
    }
    
    node* t   = (node*)((unsigned long)q->block->start + q->block->offset);
    t->data   = data;
    t->next   = NULL;
    t->prev   = NULL;
    t->offset = q->block->offset;
    
    q->block->offset += sizeof(node);
    
    if (q->block->offset >= q->block->max_size) {
        //内存整理
        _mem_move(q->block);
    }
    
    return t;
}

/**
 * 释放一个节点，其实只需要释放节点的数据就可以了，因为节点都在内存池里面，无需操心内存问题
 */
void free_node(node* n, void (*free_data)(void*))
{
    if (n->data) {
        free_data(n->data);
    }
}



