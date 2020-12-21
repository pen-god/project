#include "wrap.h"

void *start_routine(void *arg)
{
	int tcp_socket = *(int *)arg;
	printf("tcp_socket:%d\n",tcp_socket);
	while(1)
	{
		char buf[1024]={0};
		scanf("%s",buf);	
		printf("发送：%s\n",buf);
		write(tcp_socket,buf,strlen(buf));
	}
	
}

int main()
{
	//1.创建 TCP  通信协议
	int tcp_socket = Socket(AF_INET,SOCK_STREAM,0);//tcp协议
	printf("tcp_socket:%d\n",tcp_socket);
	
	int on=1;
	Setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEADDR,&on,4);//设置IP复用
	Setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEPORT,&on,4);//设置端口复用
	
	//设置链接的服务器地址信息 
	struct sockaddr_in  addr;  
	addr.sin_family   = AF_INET; //IPV4 协议  
	addr.sin_port     = htons(9301); //端口
	addr.sin_addr.s_addr = inet_addr("192.168.22.173"); //绑定所有本地网卡
	int ret = connect(tcp_socket,(struct sockaddr *)&addr,sizeof(addr));
	if(ret ==-1)
	{
		perror("connect:");
	}
	else
		printf("连接成功\n");
	
	pthread_t tid;
	pthread_create(&tid,NULL,start_routine,&tcp_socket);
	while(1)
	{
		char buf[1024]={0};
		read(tcp_socket,buf,sizeof(buf));
		printf("收到：%s\n",buf);			
		
	}
	
	
	
}

