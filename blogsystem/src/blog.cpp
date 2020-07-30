#include"database.h"
#include"httplib.h"

#define WWWROOT "./www"

 blogsystem::blogTable* table_blog; 
 blogsystem::userTable* table_user;
 blogsystem::tagTable*  table_tag;

  void Insert_user(const httplib::Request& req, httplib::Response& res)
{
	//用户信息存储于rep请求中，是json字符串
	//获取json字符串，解析为Json::Value对象
	Json::Reader reader;
	Json::Value  root;
	bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Insert user info failed,info format error\n");
	   res.status=400;
	   return ;
	}
	//执行数据库的相应操作
	 ret=table_user->Insert(root);
	if(ret==false)
	{
           printf("Insert user info failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
 	//填充相应信息
	res.status=200;
	return ;
}
  void Delete_user(const httplib::Request& req, httplib::Response& res)
{
	//从请求信息中获取用户ID
	//执行相应的数据库操作
	//填充相应信息返回
	int user_id = std::stoi(req.matches[1]);
	bool ret=table_user->Delete(user_id);
	if(ret==false)
	{
           printf("Delete user  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
         res.status=200;
	return ;
}
  void Update_user(const httplib::Request& req, httplib::Response& res)
{
	int user_id = std::stoi(req.matches[1]);
        Json::Reader reader;
	Json::Value  root;
        bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Update user info failed, info format error\n");
	   res.status=400;
	   return ;
        }
        ret=table_user->Update(user_id,root);
	if(ret==false)
	{
           printf("Update user  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	res.status=200;
	return ;
}
  void Get_all_user(const httplib::Request& req, httplib::Response& res)
{
	//调用数据库相应的操作接口，获取全部的用户信息
	//将用户信息解析为json字符串
	//将所有信息填入相应正文中返回
	Json::Value root;
	bool ret=table_user->Get_all(&root);
	if(ret==false)
	{
           printf("Get all user info  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}
  void Get_one_user(const httplib::Request& req, httplib::Response& res)
{
	int user_id=std::stoi(req.matches[1]);
	Json::Value root;
	bool ret=table_user->Get_one(user_id,&root);
	if(ret==false)
	{
           printf("Get one user info  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}

  void Insert_tag(const httplib::Request& req, httplib::Response& res)
{
	Json::Reader reader;
	Json::Value  root;
	bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Insert tag info failed,info format error\n");
	   res.status=400;
	   return ;
	}
	 ret=table_tag->Insert(root);
	if(ret==false)
	{
           printf("Insert tag info failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
 	//填充相应信息
	res.status=200;
	return ;
}
  void Delete_tag(const httplib::Request& req, httplib::Response& res)
{
	int tag_id = std::stoi(req.matches[1]);
	bool ret=table_tag->Delete(tag_id);
	if(ret==false)
	{
           printf("Delete tag  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
         res.status=200;
	return ;
}
  void Update_tag(const httplib::Request& req, httplib::Response& res)
{
	int tag_id = std::stoi(req.matches[1]);
        Json::Reader reader;
	Json::Value  root;
        bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Update tag info failed, info format error\n");
	   res.status=400;
	   return ;
        }
        ret=table_tag->Update(tag_id,root);
	if(ret==false)
	{
           printf("Update tag  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	res.status=200;
	return ;
}
  void Get_all_tag(const httplib::Request& req, httplib::Response& res)
{
	Json::Value root;
	bool ret=table_tag->Get_all(&root);
	if(ret==false)
	{
           printf("Get all tag info  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}
  void Get_one_tag(const httplib::Request& req, httplib::Response& res)
{       
	int tag_id=std::stoi(req.matches[1]);
	Json::Value root;
	bool ret=table_tag->Get_one(tag_id,&root);
	if(ret==false)
	{
           printf("Get all tag info  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}

  void Insert_blog(const httplib::Request& req, httplib::Response& res)
{
	Json::Reader reader;
	Json::Value  root;
	bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Insert blog info failed,info format error\n");
	   res.status=400;
	   return ;
	}
	 ret=table_blog->Insert(root);
	if(ret==false)
	{
           printf("Insert blog info failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
 	//填充相应信息
	res.status=200;
	return ;
}
  void Delete_blog(const httplib::Request& req, httplib::Response& res)
{
	int blog_id = std::stoi(req.matches[1]);
	bool ret=table_blog->Delete(blog_id);
	if(ret==false)
	{
           printf("Delete blog  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
         res.status=200;
	return ;
}
  void Update_blog(const httplib::Request& req, httplib::Response& res)
{
	int blog_id = std::stoi(req.matches[1]);
        Json::Reader reader;
	Json::Value  root;
        bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
           printf("Update blog info failed, info format error\n");
	   res.status=400;
	   return ;
        }
        ret=table_blog->Update(blog_id,root);
	if(ret==false)
	{
           printf("Update blog  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	res.status=200;
	return ;
}
  void Get_all_blog(const httplib::Request& req, httplib::Response& res)
{
	
	Json::Value root;
	if(req.has_param("tag_id"))
	{
	   int tag_id=std::stoi(req.get_param_value("tag_id"));	
	   bool ret=table_blog->Get_tag_blog(tag_id,&root);
	   if(ret==false)
	   {
             printf("Get tag_id:%d blog info  failed,database operator failed\n",tag_id);
	     res.status=500;
	     return ;
           }
	}
	else if(req.has_param("user_id"))
	{
	   int user_id=std::stoi(req.get_param_value("user_id"));	
	   bool ret=table_blog->Get_user_blog(user_id,&root);
	   if(ret==false)
	   {
             printf("Get user_id:%d blog info  failed,database operator failed\n",user_id);
	     res.status=500;
	     return ;
           }
	}
	else{

	    bool ret=table_blog->Get_all_blog(&root);
	   if(ret==false)
	   {
             printf("Get all blog info  failed,database operator failed\n");
	     res.status=500;
	     return ;
           }
	}
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}
  void Get_one_blog(const httplib::Request& req, httplib::Response& res)
{
	int blog_id=std::stoi(req.matches[1]);
	Json::Value root;
	bool ret=table_blog->Get_one_blog(blog_id,&root);
	if(ret==false)
	{
           printf("Get the blog info  failed,database operator failed\n");
	   res.status=500;
	   return ;
        }
	Json::FastWriter writer;
	std::string body;
        body=writer.write(root);
	res.set_content(body.c_str(),body.size(),"appliction/json");
	res.status=200;
	return ;
}
int main()
{
  MYSQL* mysql= blogsystem::_mysql_init();
  if(mysql==nullptr)
  {
        return -1;
  }
  table_blog =new blogsystem::blogTable(mysql); 
  
  table_user = new blogsystem::userTable(mysql);
  table_tag=new blogsystem::tagTable(mysql);
  
  httplib::Server svr;
   svr.set_base_dir(WWWROOT);
 //用户的增删查改操作请求
  svr.Post(R"(/user)",Insert_user);
  svr.Delete(R"(/user/(\d+))",Delete_user);
  svr.Put(R"(/user/(\d+))",Update_user);
  svr.Get(R"(/user)",Get_all_user);
  svr.Get(R"(/user/(\d+))",Get_one_user);
 //博客标签的增删查改操作请求
  svr.Post(R"(/tag)",Insert_tag);
  svr.Delete(R"(/tag/(\d+))",Delete_tag);
  svr.Put(R"(/tag/(\d+))",Update_tag);
  svr.Get(R"(/tag)",Get_all_tag);
  svr.Get(R"(/tag/(\d+))",Get_one_tag);
 //博客的增删查改操作请求
  svr.Post(R"(/blog)",Insert_blog);
  svr.Delete(R"(/blog/(\d+))",Delete_blog);
  svr.Put(R"(/blog/(\d+))",Update_blog);
  svr.Get(R"(/blog)",Get_all_blog);
  svr.Get(R"(/blog/(\d+))",Get_one_blog);
  
  svr.listen("0.0.0.0",9777);
  blogsystem:: _mysql_realease(mysql);
   //删除掉new出来的东西

  return 0;	
}
