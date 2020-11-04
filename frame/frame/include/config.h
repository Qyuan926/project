#ifndef __CONFIG_H__
#define __CONFIG_H__

//程序启动的时候，加载配置文件到内存中
void load(const char* config_name);

//获取配置命令字符串的配置项
const char* get_string(const char* key);
//获取配置命令整形的配置项
int get_int(const char* key,const int def);

void Removeleft(char* str);
void Removeright(char* str);

#endif //__CONFIG_H__


