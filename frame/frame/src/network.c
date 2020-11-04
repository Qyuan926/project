#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/types.h>

#include<pthreadpool.h>
#include<network.h>

#define CONSIZE 7
int socket_fd=0;
int epoll_fd=0;
pthreadpool_t pool;
//连接队列
connection_t con[CONSIZE];
//空闲连接的头部
connection_t *free_connection=NULL;
struct epoll_event ev_wait[CONSIZE];

void read_request_fun(connection_t* con);

void write_request_fun(connection_t* con);

void epoll_accept_net(connection_t* con);

void epoll_add_event(int fd,int read,int write,int eventtype,connection_t* con);



int listen_init_net()
{
   socket_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 //   if(socket_fd==-1)
  // {
   //  perror("socket");
    // exit(1);
  // }
  //不会立即关闭套接字经历TIME_WAIT过程，还可以重新启用
  int op=1;
  int flag=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op));
 // if(flag==-1)
 // {
 //   perror("setsocketopt");
 //   exit(1);
 // }

  struct sockaddr_in addr;
  addr.sin_family=AF_INET;
  addr.sin_port=htons(9999);
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  
  flag=bind(socket_fd,(struct sockaddr*)(&addr),sizeof(addr));
 // if(flag==-1)
 // {
 //   perror("bind");
 //   exit(1);
 // }
  flag=listen(socket_fd,SOMAXCONN);
 // if(falg==-1)
 // {
 //   perror("listen");
 //   exit(1);
 // }
  return socket_fd;
}

connection_t* get_freeconnection(int fd)
{
   connection_t* c=free_connection;
   free_connection=c->next;
   c->socket_fd=fd;
   return c;

}

void close_connection(connection_t* con)
{
   close(con->socket_fd);
   con->socket_fd=-1;
   con->next=free_connection;
   free_connection=con;

}

//创建一个epoll
int my_epoll_init()
{
  epoll_fd=epoll_create1(EPOLL_CLOEXEC);
  if(epoll_fd==-1)
  {
    perror("epoll_create1");
    exit(1);
  }
  //初始化线程池
  init_pthreadpool(&pool,7);
  //连接池
  int i=CONSIZE;
  connection_t* c=con;
  connection_t* next=NULL;
  do
  {
     i--;	  
     c[i].next=next;
     c[i].socket_fd=-1;
     next=&c[i];
  }while(i);
  
  free_connection=next;
  c=get_freeconnection(socket_fd);
  c->rhandler=epoll_accept_net;

  //添加到红黑树
  epoll_add_event(c->socket_fd,1,0,EPOLL_CTL_ADD,c);
}

void epoll_accept_net(connection_t* con)
{
    int newfd=accept(con->socket_fd,NULL,NULL);
    if(newfd==-1)
    {
       perror("accept");
       exit(1);
    }
    connection_t* newcon=get_freeconnection(newfd);
    newcon->rhandler=read_request_fun;
    newcon->whandler=write_request_fun;
    epoll_add_event(socket_fd,1,0,EPOLL_CTL_ADD,con);
}



void epoll_add_event(int fd,int read,int write,int eventtype,connection_t* con)
{
  struct epoll_event  ev;	
  if(eventtype==EPOLL_CTL_MOD)
  {
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,socket_fd,&ev);
    ev.data.ptr=con;
    ev.events=con->status;
    if(con->status==EPOLLIN)
    {
      ev.events=EPOLLOUT;
      con->status=EPOLLOUT;
    }
    else if(con->status==EPOLLOUT)
    {
        ev.events=EPOLLIN;
	con->status=EPOLLIN;
    }
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev);
  }
  else 
  {
      if(read==1)
      {
        con->status=EPOLLIN;
	ev.events=EPOLLIN;
      }
      else if(write==1)
      {
        con->status=EPOLLOUT;
        ev.events=EPOLLOUT;
      }
      ev.data.ptr=con;
      epoll_ctl(epoll_fd,eventtype,fd,&ev);
  }

}
void run(connection_t* con)
{
    //任务处理函数	
    printf("这是run函数\n");
}

void read_request_fun(connection_t* con)
{
   int r=read(con->socket_fd,con->buff,2024);
   if(r<=0)
   {
     epoll_ctl(epoll_fd,EPOLL_CTL_DEL,con->socket_fd,NULL);
     close_connection(con);
   }
   add_task_to_pool(&pool,run,con);
}


void write_request_fun(connection_t* con)
{
   int ret= write(con->socket_fd,con->buff,strlen(con->buff));
   if(ret==-1)
   {
     perror("write");
     exit(1);
   }
   epoll_add_event(con->socket_fd,0,0,EPOLL_CTL_MOD,con);
}

void my_epoll_wait()
{
   while(1)
   {
      int ret=epoll_wait(epoll_fd,ev_wait,CONSIZE,-1);
      connection_t * con=NULL;
      for(int i=0;i<ret;++i)
      {
	int events=ev_wait[i].events;      
        con=ev_wait[i].data.ptr;
        if(events& EPOLLIN)
	{
          (con->rhandler)(con);
	}
	else if(events& EPOLLOUT)
	{
          (con->whandler)(con);
	}
        
      }
   }
}
