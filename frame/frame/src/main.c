#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<config.h>
#include<func.h>
#include<list.h>
#include<process.h>

extern char** environ;

//命令行起始位置
char** g_start_command;
//环境变量长度
int  g_envirlen;
//环境变量
char* g_envir;



int main(int argc,char** argv[])
{
	g_start_command=argv;
       //加载配置文件当中的信息	
       load("config");
       //将环境变量搬到堆上
       move_envir();
       //信号的初始化
       signal_init();
       if(setDaemon()!=1)
       {
           printf("守护进程设置失败\n");
       }
        
        //主进程停留在这
       master_process_cycle();

	return 0;
}
