//进程处理
#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<func.h>
#include<config.h>
#include<process.h>
#include<sched.h>
#include<fcntl.h>
#include<network.h>
#include<pthreadpool.h>
void work_process_cycle(int num,const char* name);
void work_process_init(int num);
void work_process(int num,int flag);
void work_processCreate(int num,char* name);

//主进程函数   :创建线程，然后停留
void master_process_cycle()
{
   //屏蔽信号，防止主进程在创建子进程的时候被信号打断
   sigset_t set;
   sigemptyset(&set);

   sigprocmask(SIG_BLOCK,&set,NULL);
   //创建子进程
   int workprocess_num=get_int("workprocess_num",1);
   work_process(workprocess_num,-1);

   sigemptyset(&set);
   set_processname("master_process:framework");
   //主进程在这里阻塞，如果有信号传来，进行处理，完毕后再次阻塞
   while(1)
   {
     sigsuspend(&set);
     //处理
     printf("sigsuspend :主进程收到信号\n");
   }

}
void work_processCreate(int num,char* name)
{
	pid_t pid=fork();
	if(pid<0)
	{
	   perror("fork");
	   exit(1);
	}
	else if(pid==0)
	{
          //子进程
	  work_process_init(num);
	  work_process_cycle(num,name);
	}
	else{
           //父进程
	}

}

void work_process(int num,int flag)
{
   
    if(flag==-1)
    {
      for(int i=0;i<num;++i)
      {
       char buf[1024]={0};
       sprintf(buf,"work_process %d :framework",i);
       work_processCreate(i,buf);
      }
    }

}
//子进程要解除信号屏蔽
//设置CPU亲和，可以提高效率
void work_process_init(int num)
{
     //解除信号屏蔽
     //主进程在创建子进程的时候，信号为阻塞，子进程也会继承为阻塞
     sigset_t set;
     sigemptyset(&set);
     sigprocmask(SIG_SETMASK,&set,NULL);
     //设置CPU亲和
     cpu_set_t cpuset;
     CPU_ZERO(&cpuset);

     CPU_SET(num%CPU_SETSIZE,&cpuset);
     sched_setaffinity(0,sizeof(cpuset),&cpuset);
}
void work_process_cycle(int num,const char* name)
{
    //子进程工作前，进行初始化工作，并更改子进程名称	
    work_process_init(num);
    set_processname(name);
    listen_net_init();
    my_epoll_init();
    while(1)
    {
       //子进程工作的地方
       my_epoll_wait();
       printf("子进程开始工作\n");
    }
}


int setDaemon()
{
    pid_t pid=fork();
    if(pid<0||pid>0)
    {
       exit(0);
    }
    else if(pid==0)
    {

    }
    if(setsid()==-1)
    {
	    perror("setsid");
	    exit(1);
    }
    umask(0);
    //改变子进程的工作路径
    chdir("/");
    int  ret=open("/dev/null",O_RDWR);
    if(ret==-1)
    {
	    perror("open");
	    exit(1);
    }
    dup2(ret,STDIN_FILENO);
    dup2(ret,STDOUT_FILENO);
    return 1;
}
