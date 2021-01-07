#pragma once
#include<jsoncpp/json/json.h>
#include<openssl/md5.h>
#include<pthread.h>
#include<unordered_map>
#include<string>
#include"log.h"
#include"tool.h"

//Session类用于产生一个session
class Session{
     public:	
	Session(Json::Value& usr_info)
	{
            _user_info=usr_info;
            _origin_str.clear();
	    //原始字符串包含用户ID和用户名
            _origin_str += _user_info["user_name"].asString();
            //_origin_str += std::to_string(_user_info["user_id"].asInt());
	    std::string tmp="session构造调用成功 ";
            tmp+= _user_info["user_name"].asString();   
	    Log_blog::setlog(Log_blog::INFO,tmp);	    

	}
	Session()
	{

	}
	~Session()
	{

	}
	std::string& Get_session()
	{
		Sum_md5();
	        //std::cout<<"原始串:"<<_origin_str<<std::endl;
		//std::cout<<"session:"<<_session_id<<std::endl;
		return _session_id;
	}
        private:
	   void Sum_md5()
	   {
             MD5_CTX ctx;
	     MD5_Init(&ctx);
             std::string user;
	     user+=" user_name:";
	     user+=_user_info["user_name"].asString();
	     int ret= MD5_Update(&ctx,_origin_str.c_str(),_origin_str.size());
             if(ret!=1)
	     {
               std::string tmp=user+"MD5_Update失败";
	       Log_blog::setlog(Log_blog::ERROR,tmp);	    
               return ;
	     }
             unsigned char md[16]={0};
	     ret=MD5_Final(md,&ctx);
             if(ret!=1)
	     {
	       std::string tmp=user+"MD5_Final失败";	     
	       Log_blog::setlog(Log_blog::ERROR,tmp);	    
               return ;
	     }
            
	     char tmp[2]={0};
	     char buf[32]={0};
	     for(int i=0;i<16;++i)
	     {
                 sprintf(tmp,"%02x",md[i]);
		 strncat(buf,tmp,2);
	     }
	       _session_id=buf;
	       std::string tmpstr=user+"成功生成session"+_session_id;	     
	       Log_blog::setlog(Log_blog::INFO,tmpstr);	    
	       return ;
	   }
//	private:
        public:
		//用户对应的session
		std::string _session_id;
		//用户信息
		Json::Value _user_info;
		//用于产生MD5的原始字符串
		std::string _origin_str;
};
//在服务器端保存建立连接的session和对应的用户信息
class Allsession{
   public:
	   Allsession()
	   {
               sessionid_session.clear();
	       pthread_mutex_init(&_mutex,nullptr);
	   }
           ~Allsession()
	   {
               pthread_mutex_destroy(&_mutex);
	   }
           bool Set_sessionid(std::string& session_id,Session& session)
	   {
                pthread_mutex_lock(&_mutex);
                sessionid_session.insert(std::make_pair(session_id,session));
                pthread_mutex_unlock(&_mutex);
	       Log_blog::setlog(Log_blog::INFO,"登录用户的session成功保存到本地");	    
                return true;
	   }
	   bool Get_session(std::string& session_id,Session* session)
	   {
		   pthread_mutex_lock(&_mutex);
	           auto it=sessionid_session.find(session_id);
		   if(it==sessionid_session.end())
		   {
			   pthread_mutex_unlock(&_mutex);
	                   Log_blog::setlog(Log_blog::FATAL,"获取用户信息失败");	    
			   return false;
		   }
		   *session=it->second;
		   pthread_mutex_unlock(&_mutex);
	            Log_blog::setlog(Log_blog::INFO,"成功获取用户信息");	    
		   return true;
	   }
    private:
	   std::unordered_map<std::string,Session> sessionid_session;
	    pthread_mutex_t _mutex;
};

 void  Get_sessionid(const httplib::Request& req,std::string* session_id)
{
     std::string session=req.get_header_value("Cookie");
     std::unordered_map<std::string,std::string> ss;
     UrlUtil::PraseBody(session, &ss);
     *session_id=ss["JSESSIONID"];
}

