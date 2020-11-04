#include"pthreadpool.h"

void init_pthreadpool(pthreadpool_t* pool,int max_pthreadnum)
{
   pthread_cond_init(&(pool->cond),NULL);
   pthread_mutex_init(&(pool->mutex),NULL);
   pool->head=NULL;
   pool->tail=NULL;
   pool->max_pthreadnum=max_pthreadnum;
   pool->cur_pthreadnum=0;
   pool->idle_pthreadnum=0;
   pool->quit_flag=0;
}

void* routine(void* arg_pool)
{
	pthreadpool_t* pool=(pthreadpool_t*)arg_pool;
	while(1)
  {
	pthread_mutex_lock(&(pool->mutex));  
        pool->idle_pthreadnum++;
	while(pool->head==NULL&&pool->quit_flag==0)
	{
	  //设置为超时等待
	  pthread_cond_wait(&(pool->cond),&(pool->mutex));	
	}
        pool->idle_pthreadnum--;
	//处理任务
	if(pool->head!=NULL)
	{
		node_t* tmp=pool->head;
		pool->head=tmp->next;
                
		pthread_mutex_unlock(&(pool->mutex));
		tmp->callback(tmp->args);
		//释放节点忽略会造成内存泄漏
		free(tmp);
		pthread_mutex_lock(&(pool->mutex));

	}
	//完成销毁线程的任务
	if(pool->quit_flag==1&&pool->head==NULL)
	{
	     pool->cur_pthreadnum--;	
	     if(pool->cur_pthreadnum==0)
	     {
                 pthread_cond_signal(&(pool->cond));
	     }
	    pthread_mutex_unlock(&(pool->mutex));
            break;
	}
	pthread_mutex_unlock(&(pool->mutex));

  }

}

void add_task_to_pool(pthreadpool_t* pool,void* (*callback)(void* args),void* args)
{
     node_t* node=(node_t*)malloc(sizeof(node_t));
     if(node==NULL)
	return ;
     memset(node,0x00,sizeof(node_t));
     node->callback=callback;
     node->args=args;
     pthread_mutex_lock(&(pool->mutex));
     //将任务放到线程池里面
     if(pool->head==NULL)
     {
	     pool->head=node;
	     pool->tail=node;
     }
     else if(pool->head!=NULL)
     {
        pool->tail->next=node;
	pool->tail=node;
     }
     if(pool->idle_pthreadnum>0)
     {
           //有空闲线程，唤醒处理任务
	   pthread_cond_signal(&(pool->cond));
     }
     else if(pool->idle_pthreadnum==0&&pool->cur_pthreadnum<pool->max_pthreadnum)
     {
	 //没有空闲线程，但是还可以在创建线程    
         pthread_t* thread;
        int ret= pthread_create(thread,NULL,routine,(void*)pool);
	if(ret==0)
	{
		perror("pthread_create");
		exit(0);
	}
	pthread_detach(*thread);
        pool->cur_pthreadnum++;
     }


     pthread_mutex_unlock(&(pool->mutex));

}

void destroy_pthreadpool(pthreadpool_t* pool)
{
    if(pool->quit_flag==1)
	    return ;
    pthread_mutex_lock(&(pool->mutex));
    pool->quit_flag==1;
    if(pool->cur_pthreadnum>0)
    {
       if(pool->idle_pthreadnum>0)
       {
         pthread_cond_broadcast(&(pool->cond));
       }
       while(pool->cur_pthreadnum>0)
       {
         pthread_cond_wait(&(pool->cond),&(pool->mutex));
       }
    }
    pthread_mutex_lock(&(pool->mutex));
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->cond));
}
