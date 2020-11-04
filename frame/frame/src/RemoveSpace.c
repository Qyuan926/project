#include<stdio.h>
#include<string.h>
#include<config.h>
void Removeleft(char* str)
{
	if(str==NULL)
	 return;
	char* ptr1=str;
	if(*ptr1!=' ')
	 return ;	
	while(*ptr1==' ')
	{
            ++ptr1;
	}
	char* ptr2=str;
	while(*ptr2!='\0')
	{
	   *ptr2++=*ptr1++;
	}
	*ptr2='\0';

}

void Removeright(char* str)
{
   if(str==NULL)
      return ;
   int len=strlen(str);
   while(len>0 && str[len-1]==' ')
   {
       len--;
   }
   str[len]='\0';

}
