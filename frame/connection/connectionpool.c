#include"connectionpool.h"
#include"connection.h"

MYSQL_RES* run_fun(MYSQL* mysql,const char*sql, int flag);

void init_connectionpool(connectionpool_t* pool,const char*user,const char* passwd,const char* db,const int maxnum)
{
    pool->user=user;
    pool->passwd=passwd;
    pool->database=db;
    pool->head=pool->tail=NULL;
    pool->maxnum=maxnum;
    pool->curnum=curnum;
    pthread_cond_init(&(pool->cond),NULL);
    pthread_mutex_init(&(pool->mutex),NULL);
}
MYSQL_RES* run_fun(MYSQL* mysql,const char*sql, int flag)
{
    if(flag==0)
    {
      return query_db(mysql,sql);
    }
    else if(flag==1)
    {
	    update_db(mysql,sql);
	    return NULL;
    }

}
//flag=1 update   flag=0 query
void push_connectionpool(connectionpool_t* pool,const char* sql, int flag)
{
	pthread_mutex_lock(&(pool->mutex));
	if(pool->head!=NULL)
        {
	  //连接池中有空闲连接
          con_node_t* tmp=pool->head;
	  pool->head=tmp->next;

	  pthread_mutex_unlock(&(pool->mutex));
	  //处理任务
	  run_fun(tmp->mysql,sql,flag);
          pthread_mutex_lock(&(pool->mutex));
          
	  tmp->next=pool->head;
	  pool->head=tmp;

	  pthread_cond_signal(&(pool->cond));
	}
	else if(pool->head==NULL&&pool->curnum<pool->maxnum)
	{
	  //还可以创建连接
	  con_node_t* tmp=(con_node_t*)malloc(sizeof(con_node_t));
	  connect_db(&(tmp->mysql),pool->user,pool->passwd,pool->database);
          pthread_mutex_unlock(&(pool->mutex));
	  //处理任务
	  run_fun(tmp->mysql,sql,flag);
          pthread_mutex_lock(&(pool->mutex));
          tmp->next=pool->head;
	  pool->head=tmp;
	  pool->curnum++;
	  pthread_cond_signal(&(pool->cond));
	}
	else if(pool->head==NULL)
	{
         //没有空闲连接，并且不能创建新连接
	   while(pool->head==NULL)
	   {
             pthread_cond_wait(&(pool->cond),&(pool->mutex));
	   }
           con_node_t* tmp=pool->head;
	    pool->head=tmp->next;

	    pthread_mutex_unlock(&(pool->mutex));
	    //处理任务
	  run_fun(tmp->mysql,sql,flag);
           pthread_mutex_lock(&(pool->mutex));
          
	    tmp->next=pool->head;
	    pool->head=tmp;

	   pthread_cond_signal(&(pool->cond));

	}
	pthread_mutex_unlock(&(pool->mutex));
}
