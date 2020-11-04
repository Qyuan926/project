#ifndef __CONNECTIONPOOL_H__
#define __CONNECTIONPOOL_H__
#include<pthread.h>

typedef struct con_node{
  MYSQL* mysql;
  struct con_node* next;
}con_node_t;

typedef struct connectionpool{
   char* user;
   char* passwd;
   char* database;
   char* head;
   char* tail;
   int maxnum;
   int curnum;
   pthread_cond_t cond;
   pthread_mutex_t mutex;

}connectionpool_t;

#endif // __CONNECTIONPOOL_H__

