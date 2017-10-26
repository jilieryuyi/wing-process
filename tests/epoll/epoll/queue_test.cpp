//
//  queue_test.cpp
//  epoll
//
//  Created by yuyi on 17/10/26.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "wing.h"
#include "queue.hpp"


//用于释放队列数据的函数指针
//这里仅用户测试队列，这里的data就是一个char*
void free_data(void* data)
{
    if (data) {
        free(data);
    }
    data = NULL;
}

/**
 * 队列测试
 */
int queue_test()
{
    //创建一个队列
    queue* q = create_queue(SEND_QUEUE_BUF_SIZE);
    
    //源数据
    char* data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello1");
    
    //创建一个节点，这个节点的内存直接来自于上面的内存块
    node* n = create_node(q, data);
    //push到队列尾部
    push_queue(q, n);
    
    //第二个源数据
    data = (char*)malloc(6);
    memset(data, 0, 6);
    sprintf(data, "%s", "hello2");
    
    //创建第二个节点
    n = create_node(q, data);
    //将第二个节点push到队列尾部
    push_queue(q, n);
    
    exit_if(q->length != 2, "队列出错");
    
    printf("queue length = %lu\n", q->length);
    
    //队列遍历操作
    node* c = q->first;
    printf("======================\n");
    while(c->next != NULL) {
        printf("%s\n", c->data);
        c = c->next;
    }
    if (c) {
        printf("%s\n", c->data);
    }
    //遍历操作-end
    
    printf("======================\n");
    
    printf("offset = %lu\n", q->block->offset);
    
    //pop一个元素
    node* n1 = pop_queue(q);
    printf("%s\n", n1->data);
    printf("%lu\n", q->block->empty_offset);
    
    //执行内存对齐操作
    _mem_move(q->block);
    
    //确认对齐结果
    node* n2 = (node*)q->block->start;
    printf("%ld ==> %s\n", q->block->offset, n2->data);
    printf("%lu\n", q->block->empty_offset);
    
    //释放队列和内存块
    free_queue(q, free_data);
    
    return 0;
}

