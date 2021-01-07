

create database  if not exists blogsystem;

use  blogsystem;

create table if not exists table_user(
     user_id int primary key auto_increment comment '用户ID',
     user_name varchar(27) unique key comment '用户昵称',
     user_password varchar(27)  comment  '用户密码'
);

create table if not exists table_tag(
     user_id int  comment '用户id',
     tag_id int primary key auto_increment comment '标签ID',
     tag_name varchar(27)  comment '标签名称',
     foreign key (user_id) references table_user(user_id)
);




create table if not exists table_blog(
   blog_id int primary key auto_increment comment '博客ID' ,
   user_id int comment '用户id',
   tag_id  int comment '标签ID', 
   title varchar(255) comment '博客名称',
   content text comment '博客内容',
   last_post_time datetime comment '最后更改时间',
   foreign key (user_id) references table_user(user_id),
   foreign key (tag_id) references table_tag(tag_id)
);
