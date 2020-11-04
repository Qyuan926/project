#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<global.h>

extern char** environ;

void move_envir()
{
   //计算出环境变量的大小
    for(int i=0;environ[i]!=NULL;++i)
    {
      g_envirlen+=strlen(environ[i])+1;
    }
    
    g_envir=(char*)malloc(sizeof(char)*g_envirlen);
    memset(g_envir,0x00,sizeof(char)*g_envirlen);

    //将环境变量移动堆上，防止命令行参数过长，覆盖环境变量
    char* tmp=g_envir;
    for(int i=0;environ[i]!=NULL;++i)
    {
	int len=strlen(environ[i])+1;
	strncpy(tmp,environ[i],len);
	environ[i]=tmp;
	tmp+=len;
    }
}

void set_processname(const char* name)
{
     int namelen=strlen(name)+1;
     int commandlen=0;
     for(int i=0;g_start_command[i]!=NULL;++i)
     {
           commandlen+=strlen(g_start_command[i])+1;
     }
     
     //标题长度超出所有的空间，就不在设置标题
     if(namelen>commandlen+g_envirlen)
	     return ;
     
     g_start_command[1]=NULL;
     char* tmp=g_start_command[0];
     
     strncpy(tmp,name,namelen);
     memset(tmp+namelen,0x00,g_envirlen+commandlen-namelen);

}


