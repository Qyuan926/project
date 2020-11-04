#include"connection.h"
#include<stdio.h>
#include<stdlib.h>

int connect_db(MYSQL** mysql,char* user,char* passwd,char* db);
{
   *mysql=mysql_init(NULL);
   if(*mysql==NULL)
   {
     perror("mysql_init");
     return 0;
   }
  if(NULL== mysql_real_connect(*mysql,"localhost",user,passwd,db,0,NULL,0))
	  return 0;

  return 1;
}
int update_db(MYSQL* mysql,char* sql)
{
	if(mysql_query(mysql,sql))
	 {
           printf("updte_db fail:%s\n",mysql_error(mysql));
	   return 0;
	 }
	return 1;
}

MYSQL_RES* query_db(MYSQL* mysql,char* sql);
{
   
	if(mysql_query(mysql,sql))
	 {
           printf("query_db fail:%s\n",mysql_error(mysql));
	   return NULL;
	 }
        return mysql_store_result(mysql);
}

