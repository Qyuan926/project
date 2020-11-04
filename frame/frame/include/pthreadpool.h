#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
//线程处理的任务节点
typedef struct node {
   void* (*callback)(void* args);
   void* args;
   struct node* next;
}node_t;
//线程池结构体
typedef struct {
  //处理的节点	
  node_t *head;
  node_t *tail;
  //
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  //最大的线程数量
  int max_pthreadnum;
  //当前的线程数量
  int cur_pthreadnum;
  //空闲的线程数量
  int idle_pthreadnum;
  //是否销毁线程池
  int quit_flag;
}pthreadpool_t;


void init_pthreadpool(pthreadpool_t* pool,int max_pthreadnum);
void add_task_to_pool(pthreadpool_t* pool,void* (*callback)(void* args),void* args);
void destroy_pthreadpool(pthreadpool_t* pool);

#endif //__THREADPOOL_H__
