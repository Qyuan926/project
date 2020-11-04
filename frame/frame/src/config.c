#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<config.h>
#include<list.h>

static list_t* head=NULL;

void load(const char* config)
{
     if(config==NULL)
	     return ;
     //打开配置文件，如果出现错误即函数返回NULL，则无法加载，退出
     FILE* ret=fopen(config,"r");
     if(ret==NULL)
	return;

     //命令存储数组
     char linechar[501];
     //feof函数，如果当前位置不是文件末尾返回0；是文件末尾则返回非0
     while(!feof(ret))
     {
	memset(linechar,0x00,sizeof(linechar));
	//本行配置信息读取失败，直接读取下一行
        if(fgets(linechar,500,ret)==NULL)
	 continue;
	//遇到空串、注释等内容也直接读取下一行
        if(linechar[0]=='\0' ||linechar[0]==' ' || linechar[0]=='\n' 
	  || linechar[0]=='\t' || linechar[0]=='\r' ||linechar[0]=='#'
	  || linechar[0]=='['  )
		continue;
         
	
	while(linechar[strlen(linechar)-1]=='\n' ||linechar[strlen(linechar)-1]=='\r' ||
	      linechar[strlen(linechar)-1]==' ' 		)
          {
		  linechar[strlen(linechar)-1]=='\0';
	  }
        if(linechar[0]=='\0')
		continue;
         
	//解析配置命令
	
         head=list_init();
         if(head==NULL)
	   return ;
	char* ptr=strchr(linechar,'=');
	if(ptr!=NULL)
	{
	  char key[500]={0};
	  char val[500]={0};
          strncpy(key,linechar,(int)(ptr-linechar));
	  strcpy(val,ptr+1);
	  //去除不合法的空格
          Removeleft(key);
	  Removeright(key);
	  Removeleft(val);
	  Removeright(val);

	  //存入链表当中
	  if(!list_insert(head,key,val))
	  {
		  printf("配置信息存入链表失败\n");
		  return ;
	  }
	}
         
     }
    //关闭配置文件
    fclose(ret);
}

const char* get_string(const char* key)
{
     if(key==NULL)
	return NULL;
     node_t * cur=head->head;
     while(cur->next==NULL)
     {
	     if(strcmp(cur->key,key)==0)
	     {
                return cur->val;
	     }
	     cur=cur->next;
     }
     return NULL;
}

int get_int(const char* key,const int def)
{
   int ret=def;
   node_t* cur=head->head;
   while(cur->next==NULL)
   {
     if(strcmp(cur->key,key)==0)
     {
	     ret=atoi(cur->val);
	     break;
     }
     cur=cur->next;
   }
  return ret;
}
