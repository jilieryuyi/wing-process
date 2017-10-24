//
//  queue.cpp
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include "queue.hpp"
#include "stdlib.h"

//创建一个队列
queue* create_queue()
{
    queue *q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    q->last = NULL;
    q->length = 0;
    return q;
}

//释放一个队列
void free_queue(queue* q, void (*free_data)(void*))
{
    if (!q) {
        return;
    }
    
    node* current = q->first;
    node* temp    = NULL;
    
    while (current->next != NULL) {
        temp = current->next;
        free_node(current, free_data);
        q->length--;
        current = temp;
    }
    
    if (current) {
        free_node(current, free_data);
        q->length--;
        current = NULL;
    }
    
    free(q);
}

//追加元素到队列尾部
void push(queue* q, node *n)
{
    if (!q) {
        return;
    }
    node *t = q->last;
    
    if (t == NULL) {
        q->last  = n;
        q->first = n;
        q->length++;
        return;
    }
    
    
    t->next = n;
    q->last = n;
    q->length++;
}

//从队列首部弹出一个元素
node* pop(queue* q)
{
    if (!q) {
        return NULL;
    }
    node *t  = q->first;
    q->first = t->next;
    q->length--;
    return t;
}

//获取队列长度
unsigned long length(queue* q)
{
    return q->length;
}

//创建一个节点
node* create_node(void* data)
{
    node* t = (node*)malloc(sizeof(node));
    t->data = data;
    t->next = NULL;
    return t;
}

//释放一个节点
void free_node(node* n, void (*free_data)(void*))
{
    if (n->data) {
        free_data(n->data);
    }
    free(n);
}



