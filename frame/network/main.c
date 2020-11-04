#include<stdio.h>
#include"network.h"
#include"pthreadpool.h"
#include<stdlib.h>



int main()
{
  listen_init_net();
 my_epoll_init();
  my_epoll_wait();

return 0;	
}
