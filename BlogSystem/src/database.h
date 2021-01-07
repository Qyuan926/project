#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<string>
#include<mutex>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>
#include"log.h"
#define mysql_host     "127.0.0.1"
#define mysql_user     "root"
#define mysql_password "7"
#define mysql_database "blogsystem"


namespace blogsystem{
   static std::mutex _mutex;
  static MYSQL* _mysql_init()
  {       
	  //初始化mysql操作句柄
	  MYSQL* mysql=mysql_init(nullptr);
	   if(mysql==nullptr)
	   {
	      Log_blog::setlog(Log_blog::FATAL,"数据库初始化失败");	   
              return nullptr;
	   }
	   //连接数据库
	   if(mysql_real_connect(mysql,mysql_host,mysql_user,mysql_password,mysql_database,0,nullptr,0)==nullptr)
	   {
	      std::string str="数据库连接失败，原因：";
	      str+=mysql_error(mysql);
		   //printf("mysql connect failed,error: %s\n",mysql_error(mysql));
	      Log_blog::setlog(Log_blog::FATAL,str);
	      mysql_close(mysql);
	      return nullptr;
	   }
           
           //设置字符集
           if(mysql_set_character_set(mysql,"utf8")!=0)
	   {
		std::string str="数据库字符集设置失败，原因:";
		str+=mysql_error(mysql);
		Log_blog::setlog(Log_blog::FATAL,str);
		mysql_close(mysql);
		return nullptr;
	   }
	   return mysql;
  }

  static void _mysql_realease(MYSQL* mysql)
  {
	//mysql为有效句柄，需要进行关闭  
       if(mysql!=nullptr)
       {
	   mysql_close(mysql);
       }
       return ;
  }
  static bool _mysql_query(MYSQL* mysql,const std::string str)
  {
        if(mysql_query(mysql,str.c_str())!=0)
	{   
	    std::string tmp="数据库操作失败：";
	    tmp+=str;
	    tmp+=mysql_error(mysql);
	    Log_blog::setlog(Log_blog::WARNING,tmp);
            return false;
	}
	return true;
  }
 class blogTable{
   public:
	 blogTable(MYSQL* mysql)
	     :_mysql(mysql)
     {}
         bool Insert(const Json::Value& blog)
	 {
#define INSERT_BLOG "insert into table_blog (user_id,tag_id,title,content,last_post_time) values (%d,%d,'%s','%s',now());"
           char tmp[9999]={0};
	   sprintf(tmp,INSERT_BLOG,std::stoi(blog["user_id"].asCString()),blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString());
          //char tmp[]="insert into table_blog (user_id,tag_id,title,content,last_post_time) values(1,2,'出错了没','没有',now());"; 
	 // printf("%s\n",tmp);
	   //printf("user_id:%s\n",blog["user_id"].asCString());
	   //printf("user_id:%d\n",std::stoi(blog["user_id"].asCString()));
	  // printf("tag_id:%d\n",blog["tag_id"].asInt());
	  // printf("title:%s\n",blog["title"].asCString());
	  // printf("content:%s\n",blog["content"].asCString());
	   
	   std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
           return true;
	 }
	 //根据博客ID，删除指定的博客
	 bool Delete(const int blog_id)
	 {
 #define  DELETE_BLOG "delete from table_blog where blog_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,DELETE_BLOG,blog_id);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
           return true;

	 }
	 //修改指定的博客内容
	 bool Update(const int blog_id,Json::Value& blog)
	 {
#define UPDATE_BLOG "update table_blog set tag_id=%d,title='%s', content='%s' where blog_id=%d;"
           char tmp[4096]={0};
	   //char tmp[4096]="update table_blog set tag_id=2,title='出错了吗',content='恩' where blog_id=4;";
	   //printf("tmp\n");
	   sprintf(tmp,UPDATE_BLOG,blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString(),blog_id);
	   //printf("tag_id:%d\n",std::stoi(blog["tag_id"].asCString()));
	   //printf("tag_id:%d\n",blog["tag_id"].asInt());
	   //printf("title:%s\n",blog["title"].asCString());
	   //printf("content:%s\n",blog["content"].asCString());
	   
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
           return true;
	 }
	 //获取所有的博客
         bool Get_all_blog(Json::Value* out_blog)
	 {
#define GET_ALL_BLOG "select * from table_blog;"
           std::lock_guard<std::mutex> lg(_mutex);
	  bool ret=_mysql_query(_mysql,GET_ALL_BLOG);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value blog;
              blog["blog_id"]=std::stoi(row[0]);
	      blog["user_id"]=std::stoi(row[1]);
	      blog["tag_id"]=std::stoi(row[2]);
	      blog["title"]=row[3];
	      blog["content"]=row[4];
	      blog["last_post_time"]=row[5];
	      out_blog->append(blog);
	   }
           mysql_free_result(res);
           return true;
	 }
	 //获取指定用户的所有博客
	 bool Get_user_blog(const int user_id,Json::Value* out_blog)
	 {
 #define   GET_USER_BLOG "select * from table_blog where user_id=%d;" 		 
           char tmp[4096]={0};
	   sprintf(tmp,GET_USER_BLOG,user_id);
           std::lock_guard<std::mutex> lg(_mutex);
	  bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     std::string tmp="将查询的用户博客结果保存本地失败,name：";
	     tmp+=std::to_string(user_id);
	     Log_blog::setlog(Log_blog::ERROR,tmp);	   
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
	     std::string tmp="没有该用户博客信息,name：";
	     tmp+=std::to_string(user_id);
	     Log_blog::setlog(Log_blog::WARNING,tmp);	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value blog;
              blog["blog_id"]=std::stoi(row[0]);
	      blog["user_name"]=row[1];
	      blog["tag_id"]=std::stoi(row[2]);
	      blog["title"]=row[3];
	      blog["content"]=row[4];
	      blog["last_post_time"]=row[5];
	      out_blog->append(blog);
	   }
           mysql_free_result(res);
           return true;

	 }
	 //获取指定标签的所有博客
	 bool Get_tag_blog(const int tag_id,Json::Value* out_blog)
	 {
 #define   GET_TAG_BLOG "select * from table_blog where tag_id=%d;" 		 
           char tmp[4096]={0};
	   sprintf(tmp,GET_TAG_BLOG,tag_id);
           std::lock_guard<std::mutex> lg(_mutex);
	  bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value blog;
              blog["blog_id"]=std::stoi(row[0]);
	      blog["user_id"]=std::stoi(row[1]);
	      blog["tag_id"]=std::stoi(row[2]);
	      blog["title"]=row[3];
	      blog["content"]=row[4];
	      blog["last_post_time"]=row[5];
	      out_blog->append(blog);
	   }
           mysql_free_result(res);
           return true;  

	 }
	 bool Get_one_blog(const int blog_id,Json::Value* out_blog)
	 {
 #define   GET_ONE_BLOG "select * from table_blog where blog_id=%d;" 		 
           std::lock_guard<std::mutex> lg(_mutex);
           char tmp[4096]={0};
	   sprintf(tmp,GET_ONE_BLOG,blog_id);
	  bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!=1)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value blog;
              blog["blog_id"]=std::stoi(row[0]);
	      blog["user_id"]=std::stoi(row[1]);
	      blog["tag_id"]=std::stoi(row[2]);
	      blog["title"]=row[3];
	      blog["content"]=row[4];
	      blog["last_post_time"]=row[5];
	      out_blog->append(blog);
	   }
           mysql_free_result(res);
           return true;  
        }
   private: 
       MYSQL* _mysql;	   
 };
 class tagTable{
   public:
        tagTable(MYSQL* mysql)
	 :_mysql(mysql)	
	 {}

	bool Insert(const Json::Value& tag)
	{
   #define INSERT_TAG  "insert into table_tag (user_id,tag_name) values (%d,'%s');"
	   char tmp[4096]={0};
	   sprintf(tmp,INSERT_TAG,std::stoi(tag["user_id"].asCString()),tag["tag_name"].asCString());
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
	   return false;
	   }
	   return true;
	}
        bool Delete(int tag_id)
	{
  #define DELETE_TAG "delete from table_tag where tag_id=%d;"
	    char tmp[4096]={0 };
	    sprintf(tmp,DELETE_TAG,tag_id);
           std::lock_guard<std::mutex> lg(_mutex);
	    bool ret=_mysql_query(_mysql,tmp);
	    if(ret==false)
	    {
		    return false;
	    }
           return true;
	}
	bool Update(int tag_id,const Json::Value& tag)
	{
   #define UPDATE_TAG "update table_tag set tag_name='%s' where tag_id=%d;"
	    char tmp[4096]={0};
	    sprintf(tmp,UPDATE_TAG,tag["tag_name"].asCString(),tag_id);
           std::lock_guard<std::mutex> lg(_mutex);
	    bool ret=_mysql_query(_mysql,tmp);
	    if(ret==false)
	    {
		    return false;
	    }
            return true;
	}
       bool Get_all( Json::Value* out_tag)
       {
   #define  GET_ALL_TAG "select * from table_tag;"
           std::lock_guard<std::mutex> lg(_mutex);
            bool ret=_mysql_query(_mysql,GET_ALL_TAG);
	    if(ret==false)
	    {
		    return false;
	    }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<0)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value tag;
              tag["user_id"]=std::stoi(row[0]);
              tag["tag_id"]=std::stoi(row[1]);
	      tag["tag_name"]=row[2];
	      out_tag->append(tag);
	   }
           mysql_free_result(res);
	   return true;
       }
       bool Get_one(int tag_id,Json::Value* out_tag)
       {
#define GET_ONE_TAG "select * from table_tag where tag_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,GET_ONE_TAG,tag_id);
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<0)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value tag;
              tag["user_id"]=std::stoi(row[0]);
	      tag["tag_id"]=std::stoi(row[1]);
	      tag["tag_name"]=row[2];
	      out_tag->append(tag);
           mysql_free_result(res);
	   return true;

       }
       bool Get_tag_byuser(int user_id,Json::Value* out_tag)
       {
#define GET_ONE_TAG_USER "select * from table_tag where user_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,GET_ONE_TAG_USER,user_id);
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
	     return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<0)
	   {
	     Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value tag;
              tag["user_id"]=std::stoi(row[0]);
              tag["tag_id"]=std::stoi(row[1]);
	      tag["tag_name"]=row[2];
	      out_tag->append(tag);
	   }
           mysql_free_result(res);
	  // printf("get tag info success tag_id:%d\n",tag_id);
	   return true;

       }
   private:
	   MYSQL* _mysql;
 };
 class userTable{
   public:
       userTable(MYSQL* mysql)
	  : _mysql(mysql)
       {}
       bool Insert(const Json::Value& user)
       {
#define INSERT_USER "insert into table_user values (null,'%s','%s');"
           char tmp[4096]={0};
	   sprintf(tmp,INSERT_USER,user["user_name"].asCString(),user["user_password"].asCString());
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
	  return true;     
       }
       bool Delete(const int user_id)
       {
#define DELETE_USER "delete from table_user where user_id=%d;"
#define DELETE_USER_TAG "delete from table_tag where user_id=%d;"
#define DELETE_USER_BLOG "delete from table_blog where user_id=%d;"
           std::lock_guard<std::mutex> lg(_mutex);
           char start[]="set foreign_key_checks=0;";
	   bool ret=_mysql_query(_mysql,start);
	   if(ret==false)
	   {
		   return false;
	   }

	   char tmp[4096]={0};
	   sprintf(tmp,DELETE_USER,user_id);
	   ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
	   char tmps[4096]={0};
	   sprintf(tmps,DELETE_USER_TAG,user_id);
	   ret=_mysql_query(_mysql,tmps);
	   if(ret==false)
	   {
		   return false;
	   }
	   char tmpb[4096]={0};
	   sprintf(tmpb,DELETE_USER_BLOG,user_id);
	   ret=_mysql_query(_mysql,tmpb);
	   if(ret==false)
	   {
		   return false;
	   }
	  
           char end[]="set foreign_key_checks=0;";
	   ret=_mysql_query(_mysql,end);
	   if(ret==false)
		   return false;
          
	   
	  return true;     

       }
       bool Update(const int user_id,const Json::Value& user)
       {
#define UPDATE_USER "update table_user set user_name='%s'  where user_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,UPDATE_USER,user["user_name"].asCString(),user_id);
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
	  return true;     

       }
       bool Get_all( Json::Value* out_user)
       {
   #define  GET_ALL_USER "select * from table_user;"
           std::lock_guard<std::mutex> lg(_mutex);
            bool ret=_mysql_query(_mysql,GET_ALL_USER);
	    if(ret==false)
	    {
		    return false;
	    }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
	      Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value user;
              user["user_id"]=std::stoi(row[0]);
	      user["user_name"]=row[1];
	      out_user->append(user);
	   }
           mysql_free_result(res);
	   return true;
       }
      // bool Get_one(std::string& user_name ,Json::Value* out_user)
       bool Get_one(std::string& user_name ,int* user_id)
       {
#define GET_ONE_USER "select * from table_user where user_name=%s;"
           char tmp[4096]={0};
	   sprintf(tmp,GET_ONE_USER,user_name.c_str());
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!=1)
	   {
	      Log_blog::setlog(Log_blog::WARNING,"查询结果不符合预期");	   
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   MYSQL_ROW row=mysql_fetch_row(res);   
	   Json::Value user;
              user["user_id"]=std::stoi(row[0]);
	      user["user_name"]=row[1];
	      *user_id=std::stoi(row[0]);
           mysql_free_result(res);
	   return true;

       }
       
       bool Login(Json::Value& user,Json::Value* out_user)
       {
#define LO_ONE_USER "select * from table_user where user_name='%s';"
           char tmp[4096]={0};
	   sprintf(tmp,LO_ONE_USER,user["user_name"].asCString());
           std::lock_guard<std::mutex> lg(_mutex);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
	     Log_blog::setlog(Log_blog::ERROR,"将数据库结果保存到本地失败");	   
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!=1)
	   {
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   MYSQL_ROW row=mysql_fetch_row(res);   
	   Json::Value tmp_user;
              tmp_user["user_id"]=std::stoi(row[0]);
	      tmp_user["user_name"]=row[1];
	      tmp_user["user_password"]=row[2];
	      Json::Value _out_user;
	      _out_user["user_id"]=std::stoi(row[0]);
	      _out_user["user_name"]=row[1];
	      out_user->append(_out_user);
              
	     if(tmp_user["user_password"].asString()!=user["user_password"].asString())
	     {
               mysql_free_result(res);
               return false;
	     }
	   
	   
           mysql_free_result(res);
	   return true;
         }
   private:
	   MYSQL* _mysql;
 };
}
