#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<string>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>

#define mysql_host     "127.0.0.1"
#define mysql_user     "root"
#define mysql_password "7"
#define mysql_database "blogsystem"


namespace blogsystem{
 
  static MYSQL* _mysql_init()
  {       
	  //初始化mysql操作句柄
	  MYSQL* mysql=mysql_init(nullptr);
	   if(mysql==nullptr)
	   {
              printf("mysql init failed\n");
              return nullptr;
	   }
	   //连接数据库
	   if(mysql_real_connect(mysql,mysql_host,mysql_user,mysql_password,mysql_database,0,nullptr,0)==nullptr)
	   {
	      printf("mysql connect failed,error: %s\n",mysql_error(mysql));
	      mysql_close(mysql);
	      return nullptr;
	   }
           //选择数据库
	   //if(mysql_select_db(mysql,blogsystem)!=0)
	   //{
           //   printf("select mysql failed");
	   //   mysql_close(mysql);
           //   return nullptr;
	   //}
           //设置字符集
           if(mysql_set_character_set(mysql,"utf8")!=0)
	   {
                printf("set charset failed,error: %s\n",mysql_error(mysql));
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
	    printf("%s query failed,error : %s",str.c_str(),mysql_error(mysql));
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
#define INSERT_BLOG "insert into table_blog values (null,%d,%d,'%s',%s,now());"
           char tmp[4096]={0};
	   sprintf(tmp,INSERT_BLOG,blog["user_id"].asInt(),blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString());
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
#define UPDATE_BLOG "update table_blog set tag_id=%d,title=%s, content=%s where blog_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,UPDATE_BLOG,blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString(),blog_id);
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
	  bool ret=_mysql_query(_mysql,GET_ALL_BLOG);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	      mysql_free_result(res);//将结果集的资源释放掉
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
              printf("No tag information\n");
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
	  bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
              printf("No tag information\n");
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
	 //获取指定标签的所有博客
	 bool Get_tag_blog(const int tag_id,Json::Value* out_blog)
	 {
 #define   GET_TAG_BLOG "select * from table_blog where tag_id=%d;" 		 
           char tmp[4096]={0};
	   sprintf(tmp,GET_TAG_BLOG,tag_id);
	  bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
            return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
              printf("No tag information\n");
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
             printf("store result to local failed\n");
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!=1)
	   {
              printf("No blog information\n");
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
   #define INSERT_TAG  "insert into table_tag (tag_name) values ('%s');"
	   char tmp[4096]={0};
	   sprintf(tmp,INSERT_TAG,tag["tag_name"].asCString());
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
	    bool ret=_mysql_query(_mysql,tmp);
	    if(ret==false)
	    {
		    return false;
	    }
           return true;
	}
	bool Update(int tag_id,const Json::Value& tag)
	{
   #define UPDATE_TAG "update table_tag set tag_name=%s where tag_id=%d;"
	    char tmp[4096]={0};
	    sprintf(tmp,UPDATE_TAG,tag["tag_name"].asCString(),tag_id);
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
            bool ret=_mysql_query(_mysql,GET_ALL_TAG);
	    if(ret==false)
	    {
		    return false;
	    }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
              printf("No tag information\n");
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   for(int i=0;i<num_rows;++i)
	   {
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value tag;
              tag["tag_id"]=std::stoi(row[0]);
	      tag["tag_name"]=row[1];
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
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!= 1)
	   {
              printf("No tag information\n");
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
              MYSQL_ROW row=mysql_fetch_row(res);
	      Json::Value tag;
              tag["tag_id"]=std::stoi(row[0]);
	      tag["tag_name"]=row[1];
	      out_tag->append(tag);
           mysql_free_result(res);
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
#define INSERT_USER "insert into table_user values (null,'%s');"
           char tmp[4096]={0};
	   sprintf(tmp,INSERT_USER,user["user_name"].asCString());
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
           char tmp[4096]={0};
	   sprintf(tmp,DELETE_USER,user_id);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
	  return true;     

       }
       bool Update(const int user_id,const Json::Value& user)
       {
#define UPDATE_USER "updata table_user set user_name=%s where user_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,UPDATE_USER,user_id,user["user_name"].asCString());
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
            bool ret=_mysql_query(_mysql,GET_ALL_USER);
	    if(ret==false)
	    {
		    return false;
	    }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);//线程安全
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
           //统计结果集的行数
	   int num_rows=mysql_num_rows(res);
	   if(num_rows<=0)
	   {
              printf("No tag information\n");
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
       bool Get_one(int user_id,Json::Value* out_user)
       {
#define GET_ONE_USER "select * from table_user where user_id=%d;"
           char tmp[4096]={0};
	   sprintf(tmp,GET_ONE_USER,user_id);
	   bool ret=_mysql_query(_mysql,tmp);
	   if(ret==false)
	   {
		   return false;
	   }
            //将查询的结果保存到本地
           MYSQL_RES* res=mysql_store_result(_mysql);
	   if(res==nullptr)
	   {
             printf("store result to local failed\n");
	     return false;
	   }
	   int num_rows=mysql_num_rows(res);
	   if(num_rows!=1)
	   {
              printf("No tag information\n");
	      mysql_free_result(res);//将结果集的资源释放掉
	      return false;
	   }
	   MYSQL_ROW row=mysql_fetch_row(res);   
	   Json::Value user;
              user["user_id"]=std::stoi(row[0]);
	      user["user_name"]=row[1];
	      out_user->append(user);
           mysql_free_result(res);
	   return true;

       }
       
   private:
	   MYSQL* _mysql;
 };
}
