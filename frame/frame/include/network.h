#ifndef __NETWORK_H__
#define __NETWORK_H__

typedef struct connection{
	int socket_fd;//已连接的套接字
	int status;   //事件状态
	//回调函数
	void*(*rhandler)(void*);
	void*(*whandler)(void*);
	//回调函数传输的内容
	char buff[2024];
	struct connection* next;
}connection_t;
int listen_init_net();

int my_epoll_init();
void my_epoll_wait();
#endif //__NETWORK_H__
