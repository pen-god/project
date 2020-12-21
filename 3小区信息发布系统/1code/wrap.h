#ifndef WRAP_H
#define WRAP_H

#include <stdio.h>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <pwd.h>
#include <grp.h>
#include <sys/sysmacros.h>
#include <linux/input.h>
#include <stdbool.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <arpa/inet.h>

int Socket(int domain, int type, int protocol)
{
	int tcp_socket = socket(domain,type,protocol);
	if(tcp_socket < 0)
	{
		perror("");
		exit(0); 
	}
	else
		printf("创建成功\n");
	return tcp_socket;
}
	
//2.绑定服务器地址信息
int Bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	int ret = bind(sockfd,addr,addrlen);
	if(ret < 0)
	{
		perror("");
		exit(0);
	}
	else
	{
		printf("绑定成功\n");
	}
}

//3.设置服务器为监听模式 
int Listen(int sockfd, int backlog)
{
	int ret=listen(sockfd,backlog);
	if(ret < 0)
	{
		perror("");
		exit(0);
	}
	else
	{
		printf("设置监听成功\n");
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int  new_socket = accept(sockfd,addr,addrlen);
	if(new_socket < 0)
	{
		perror("链接服务器:");
		return  0;
	}
}
	
int Setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen)
{
	int r = setsockopt(sockfd,level,optname,optval,optlen);//设置IP复用
	if (r == -1)
	{
		perror("setsockopt error:");
		return -1;
	}
	else
		printf("设置套接字属性成功\n");
}

					  
#endif // WRAP_H

