#include"list.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


list_t* list_init()
{
   list_t* ptr=(list_t*)malloc(sizeof(list_t));
   if(ptr==NULL)
    return NULL;
   //将list_t的内容初始化
   memset(ptr,0x00,sizeof(list_t));
   ptr->head=NULL;
   ptr->len=0;
   return ptr;
}

int list_insert(list_t* head,const char* key,const char* val)
{
    //创建节点	
    node_t* node=(node_t*)malloc(sizeof(node_t));
    memset(node,0x00,sizeof(node_t));
    if(node==NULL)
      return 0;
    node->key=(char*)malloc(sizeof(key)+1);
    if(node->key==NULL)
      return 0;
    strncpy(node->key,key,sizeof(key)+1);
    node->val=(char*)malloc(sizeof(val)+1);
    if(node->val==NULL)
      return 0;
    strncpy(node->val,val,sizeof(val)+1);
    //头插
    if(head==NULL)
    {
       head=node;
    }
    else
    {
         node->next=head;
	 head=node;
    }
    head->len++;
    return 1;
}
