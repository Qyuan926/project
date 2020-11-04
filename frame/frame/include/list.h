#ifndef  __LIST_H__
#define  __LIST_H__

typedef struct node{
char* key;
char* val;
struct node* next;
}node_t;

typedef struct list{
  node_t* head;
  int  len;
}list_t;

list_t*  list_init();

int list_insert(list_t* head,const char* key ,const char* val);

#endif //__LIST_H__

