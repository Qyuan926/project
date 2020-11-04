#include<global.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>


//信号
typedef struct {
	int signo;        //当前信号
	char* signame;    //信号名称
	char* commandname;//命令
        void (*handler)(int signo,siginfo_t* info,void* com);
}signal_t;
//信号处理函数
        void sig_handler(int signo,siginfo_t* info,void* com);

//要处理的信号
signal_t signal_arr[]={
  {SIGHUP,    "SIGHUP",    NULL, sig_handler},
  {SIGINT,    "SIGINT",    NULL, sig_handler},
  {SIGQUIT,   "SIGQUIT",   NULL, sig_handler},
  {SIGCHLD,   "SIGCHLD",   NULL, sig_handler},
  {SIGTERM,   "SIGTERM",   NULL, sig_handler},
  {SIGSYS,    "SIGSYS",    NULL, NULL},
  {0,NULL,NULL,NULL}
};


void sig_handler(int signo,siginfo_t* info,void* com)
{
    printf("处理信号：%d\n",signo);
}
void signal_init()
{
    signal_t*  sig=NULL;
    struct sigaction  sa;

    for(sig=signal_arr;signal_arr->signo!=0;sig++)
    {
	 //如果该信号没有对应的处理函数，则该信号的处理方式为忽略SIG_ING
        if(sig->handler!=NULL)
	{ 
		//强转类型赋予sa_sigaction
          sa.sa_sigaction=(void(*)(int,siginfo_t*,void*))(SIG_IGN);
	}
	else
	{
	   //有对应的处理函数，则赋值为对应的处理函数	
           sa.sa_sigaction=sig_handler;
	   sa.sa_flags=SA_SIGINFO;
	}

       sigemptyset(&sa.sa_mask);
       //调用信号处理函数
       sigaction(sig->signo,&sa,NULL);
    }


}



