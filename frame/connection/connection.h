#ifndef _CONNECTION_H__
#define _CONNECTION_H__
#include<mysql/mysql.h>

int connect_db(MYSQL** mysql,char* user,char* passwd,char* db);

int update_db(MYSQL* mysql,char* sql);

MYSQL_RES* query_db(MYSQL* mysql,char* sql);

#endif //  _CONNECTION_H__

