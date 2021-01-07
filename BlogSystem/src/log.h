#pragma once
#include<iostream>
#include<string>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

namespace Log_blog{

   //获取时间
  class Logtime{
     public:
	 static  void  Get_logtime(std::string* logtime)
	     {
		 time_t sys_time;
		 time(&sys_time);

		 struct tm* time=localtime(&sys_time);
	         char buf[30]={0};
		 snprintf(buf,sizeof(buf)-1,"%04d年%02d月%02d日 %02d:%02d:%02d",time->tm_year+1900,time->tm_mon+1,
				            time->tm_mday,time->tm_hour,time->tm_min,time->tm_sec);
		 logtime->assign(buf,sizeof(buf));
	     }

  };
  //划分日志等级
  const char* log_grade[]={
         "INFO",
	 "WARNING",
	 "ERROR",
	 "FATAL",
	 "DEBUG"
  };
  enum  log_level{
        INFO=0,
	WARNING,
	ERROR,
	FATAL,
	DEBUG

  };
  //时间 日志等级 信息  文件  行号
 inline  void   Getloginfo(int log_level,std::string info,const char* file,int line)
  {
       std::string time;	  
       Logtime::Get_logtime(&time);
       if(log_level!=0)
       {
       FILE* fd = fopen("../bin/log/fail.text","a+");
       if(fd==NULL)
       {
       	       std::cout<<"记录log信息失败：打开文件失败"<<std::endl;    
       }
       fprintf(fd,"%s %s %s %s:%d\n",time.c_str(),log_grade[log_level],info.c_str(),file,line);
       fclose(fd);
       }
       else
       {
       FILE* fd = fopen("../bin/log/success.text","a+");
       if(fd==NULL)
       {
       	       std::cout<<"记录log信息失败：打开文件失败"<<std::endl;    
       }
       fprintf(fd,"%s %s %s %s:%d\n",time.c_str(),log_grade[log_level],info.c_str(),file,line);
       fclose(fd);

       }
  }

 
#define  setlog(log_level,info)	Getloginfo(log_level,info,__FILE__,__LINE__) 

}







