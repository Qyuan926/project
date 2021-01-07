#include"database.h"
#include"httplib.h"
#include"session.h"
#include<jsoncpp/json/json.h>
#include"log.h"
#include"tool.h"
#define WWWROOT "./www"
             Allsession* all_session;

 //blogsystem::blogTable*  table_blog; 
 blogsystem::userTable*  table_user;
 blogsystem::tagTable*   table_tag;
 blogsystem::blogTable*  table_blog; 
             //Allsession* all_session;
  
  void Login_user(const httplib::Request& req,httplib::Response& res)
  {
          Json::Reader reader;
       	  Json::Value  root;
       	  Json::Value  out_root;
          bool ret=reader.parse(req.body,root);
	  if(ret==false)
	  {
	   std::string user="登录时获取浏览器信息失败，用户名：";
	   user+=root["user_name"].asCString();
	   Log_blog::setlog(Log_blog::FATAL,user);
	   res.status=400;
	   return ;
          }
          ret=table_user->Login(root,&out_root);
	  if(ret==false)
	  {
	   std::string user="登录时获取数据库信息失败，用户名：";
	   user+=root["user_name"].asCString();
	   Log_blog::setlog(Log_blog::FATAL,user);
	   res.status=500;
	   return ;
          }
	  //std::cout<< root["user_name"].asString()<<std::endl;
	  //root["user_id"]=out_root["user_id"]; 
	
	  //获取session
          Session sess(root);
       	  std::string sess_id=sess.Get_session();
          //保存在服务端
          all_session->Set_sessionid(sess_id,sess);
	  std::string tmp="JSESSIONID = ";
	  tmp+=sess_id;
           res.set_header("Set-Cookie",tmp.c_str());
	  Json::FastWriter writer;
	  std::string body;
          body=writer.write(out_root);
	  res.set_content(body.c_str(),body.size(),"appliction/json");
	  res.status=200;

	  std::string  info="用户登录成功,session: ";
	  info+=tmp;
	  Log_blog::setlog(Log_blog::INFO,info);
	  return ;
  }

  void Insert_user(const httplib::Request& req, httplib::Response& res)
  {	
	//用户信息存储于rep请求中，是json字符串
	//获取json字符串，解析为Json::Value对象
	Json::Reader reader;
	Json::Value  root;
	bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::INFO,"用户信息格式错误，导致未注册");
	   res.status=400;
	   return ;
	}
	//执行数据库的相应操作
	 ret=table_user->Insert(root);
	if(ret==false)
	{
		
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户未注册成功");
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
	//int user_id = std::stoi(req.matches[1]);
	std::string sessid;
	Get_sessionid(req,&sessid);
	Session session;
        bool ret=all_session->Get_session(sessid,&session);
        if(ret==false)
	{
            //res.set_redirect(WWW)		
           return ;
	}
	std::string user_name=session._user_info["user_name"].asString();
        int user_id=-1;
	table_user->Get_one(user_name,&user_id);
	 ret=table_user->Delete(user_id);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::WARNING,"数据库操作错误，导致用户未注销成功");
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
	   Log_blog::setlog(Log_blog::WARNING,"数据库操作错误，导致用户信息更新失败");
	   res.status=400;
	   return ;
        }
        ret=table_user->Update(user_id,root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户信息更新失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致获取所有用户信息失败");
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
	//修改
       //	int user_id=std::stoi(req.matches[1]);
      //	Json::Value root;
       //
       //bool ret=table_user->Get_one(user_id,&root);
//	bool ret=true;
//	if(ret==false)
//	{
//	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致获取用户信息失败");
//	   res.status=500;
//	   return ;
 //       }
//	Json::FastWriter writer;
//	std::string body;
 //       body=writer.write(root);
//	res.set_content(body.c_str(),body.size(),"appliction/json");
//	res.status=200;
//	return ;
}

  void Insert_tag(const httplib::Request& req, httplib::Response& res)
{
	Json::Reader reader;
	Json::Value  root;
	bool ret=reader.parse(req.body,root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::WARNING,"标签信息错误，导致用户插入文章标签失败");
	   res.status=400;
	   return ;
	}
	 ret=table_tag->Insert(root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户插入文章标签失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户删除文章标签失败");
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
	   Log_blog::setlog(Log_blog::WARNING,"标签信息错误，导致用户更新文章标签失败");
	   res.status=400;
	   return ;
        }
        ret=table_tag->Update(tag_id,root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户更新文章标签失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取文章标签失败");
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
  void Get_all_user_tag(const httplib::Request& req, httplib::Response& res)
{
	int user_id = std::stoi(req.matches[1]);
	Json::Value root;
	bool ret=table_tag->Get_tag_byuser(user_id,&root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取文章标签失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取文章标签失败");
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
	   Log_blog::setlog(Log_blog::ERROR,"博客格式错误，导致用户提交博客失败");
	   res.status=400;
	   return ;
	}
	 ret=table_blog->Insert(root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户提交博客失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户删除博客失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"博客格式错误，导致用户更新博客失败");
	   res.status=400;
	   return ;
        }
        ret=table_blog->Update(blog_id,root);
	if(ret==false)
	{
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户更新博客失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取博客失败");
	     res.status=500;
	     return ;
           }
	}
	else if(req.has_param("user_id"))
	{
	   int user_id=std::stoi(req.get_param_value("user_id") );	
	   bool ret=table_blog->Get_user_blog(user_id,&root);
	   if(ret==false)
	   {
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取博客失败");
	     res.status=500;
	     return ;
           }
	}
	else{

	    bool ret=table_blog->Get_all_blog(&root);
	   if(ret==false)
	   {
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取博客失败");
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
	   Log_blog::setlog(Log_blog::DEBUG,"数据库操作错误，导致用户获取博客失败");
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
int main(int argc,char* argv[])
{
  if(argc!=1)
  {
	  return -1;
  }
  int port=atoi(argv[0]);
  MYSQL* mysql= blogsystem::_mysql_init();
  if(mysql==nullptr)
  {
        return -1;
  }
  table_blog =new blogsystem::blogTable(mysql);   
  table_user = new blogsystem::userTable(mysql);
  table_tag=new blogsystem::tagTable(mysql);
  all_session=new Allsession();
  
  httplib::Server svr;
   svr.set_base_dir(WWWROOT);
 //用户的增删查改操作请求
  svr.Post(R"(/login)",Login_user);
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
  svr.Get(R"(/tag_user/(\d+))",Get_all_user_tag);
 //博客的增删查改操作请求
  svr.Post(R"(/blog)",Insert_blog);
  svr.Delete(R"(/blog/(\d+))",Delete_blog);
  svr.Put(R"(/blog/(\d+))",Update_blog);
  svr.Get(R"(/blog)",Get_all_blog);
  svr.Get(R"(/blog/(\d+))",Get_one_blog);
  
  svr.listen("0.0.0.0",9777);
  blogsystem:: _mysql_realease(mysql);
   //删除掉new出来的东西
   delete table_user;
   delete table_blog;
   delete table_tag;
   delete all_session;
  return 0;	
}
