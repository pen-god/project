#include "wrap.h"
#include "list.h"

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
	addr.sin_addr.s_addr = INADDR_ANY; //绑定所有本地网卡
	
	Bind(tcp_socket,(struct sockaddr *)&addr,sizeof(addr));
	
	//3.设置服务器为监听模式 
	Listen(tcp_socket,5); 
	
	linklist head = init_list(); //申请好友列表头节点
	
	int max = tcp_socket; //记录最大文件描述符
	
	//创建描述符集合
	fd_set set;
	
	while(1)
	{		
			
		//1.把文件描述符集合清空
		FD_ZERO(&set);
		
		//2.把需要监听文件描述符 添加到集合中 
		FD_SET(0,&set);
		//2.把阻塞的文件描述符添加到集合中
		FD_SET(tcp_socket,&set);
		
		struct list_head *pos = NULL;
		list_for_each(pos, &head->list)
		{		
			linklist p;
			p = list_entry(pos, struct node, list);
			FD_SET(p->new_socket,&set); //所有链表描述符添加到集合中
			
			if(p->new_socket > max)  //获取最大的文件描述符
				max = p->new_socket;
		}
		
		//printf("max:%d\n",max);
		//3.监听集合
		printf("开始监视犯人\n");
		int ret = select(max+1,&set,NULL,NULL,NULL);//监听 
		if(ret < 0)
		{
			perror("");
			return 0;
		}
				
		if(FD_ISSET(tcp_socket,&set))  //判断是否活跃 
		{
			linklist new_cilent = new_node(false);
			socklen_t len = sizeof(new_cilent->addr);
			
			//4.接收客户端的链接请求 
			printf("等待服务器链接进来\n");
			int  new_socket = Accept(tcp_socket,(struct sockaddr *)&new_cilent->addr,&len);
			new_cilent->get_name = true;
			new_cilent->new_socket=new_socket;			
			list_add_tail(&new_cilent->list,&head->list);
			printf("处理成功 \n");			
		}
		
		//判断是否键盘输入 
		if(FD_ISSET(0,&set))
		{
			int  a=0;
			scanf("%d",&a);
			//printf("a=%d\n",a);
			if(a == 1)					//输入1 发送公告
			{
				char buf[1024]="公告:今晚八点，是兄弟就来砍我";
				printf("buf=%s",buf);
				pos = NULL;
				list_for_each(pos, &head->list)
				{		
					linklist p;
					p = list_entry(pos, struct node, list);
					write(p->new_socket,buf,strlen(buf));
				}
				printf("发送公告\n"); 			
			}
			else if(a == 2)				//输入2 显示好友列表
				show(head);
		
		}
		
		pos = NULL;
		struct list_head *n = NULL;
		list_for_each_safe(pos,n,&head->list)//遍历每一个用户，找到用户文件描述符来操作
		{				
			linklist p;
			p = list_entry(pos, struct node, list);
			if(FD_ISSET(p->new_socket,&set))
			{
				char buf[1024]={0};
				int size = read(p->new_socket,buf,sizeof(buf)); 
				if(size <= 0)//掉线处理
				{
					struct list_head *die_pos = NULL;
					list_for_each(die_pos, &head->list)
					{		
						linklist die_p;
						die_p = list_entry(die_pos, struct node, list);
						if(die_p->new_socket == p->chat_socket)
						{
							char die_buf[50]="对方已经掉线，已经退出聊天模式";
							write(die_p->new_socket,die_buf,strlen(die_buf));
							die_p->chat = false;							
						}
														
					}
					printf("客户端已经掉线了\n");
					list_del(pos);				
					close(p->new_socket);
					free(p); 
				}
				else
				{
					if(p->get_name)//获取用户账号
					{
						p->get_name = false;
						strcpy(p->name,buf);
						printf("收到name=%s\n",buf);
						//write()
					}
					else if(p->chat)//聊天模式
					{
						bool end = false;
						if(strcmp(buf,"end")==0||end)
						{
							//接受退出聊天暗号可以退出聊天
							write(p->new_socket,"发送ok退出聊天，负责代表发送end",strlen("发送ok退出聊天，负责代表发送end"));
							end = true;
							continue;
							if(strcmp(buf,"ok")==0)
							{
								p->chat=false;
								write(p->new_socket,"聊天已退出",strlen("聊天已退出"));
								continue;
							}
							
						}
							
						printf("收到信息=%s\n",buf);
						char chat_buf[1054]={0};
						sprintf(chat_buf,"%s say: %s",p->name,buf);
						write(p->chat_socket,chat_buf,strlen(chat_buf));
					}
					else
					{
						bool a = true;
						if(strstr(buf,"chat"))//接收暗号，匹配就进入聊天模式
						{
							//char ip[50]={0};
							sscanf(buf,"chat=%s",p->chat_name);
							printf("聊天对象名字%s\n",p->chat_name);
							struct list_head *chat_pos = NULL;
							list_for_each(chat_pos, &head->list)
							{		
								linklist chat_p;
								chat_p = list_entry(chat_pos, struct node, list);
								if(strstr(chat_p->name,p->chat_name))
								{
									p->chat=true;
									printf("账号匹配\n");
									write(p->new_socket,"chat OK",strlen("chat OK"));
									p->chat_socket = chat_p->new_socket;
									a = false;
									printf("chat_socket=%d\n",p->chat_socket);
									break;
								}
																
							}
							if(a)
							{
								write(p->new_socket,"chat noOK",strlen("chat no OK"));
							}
								
						}
						
					}
					
				}
					
			}
			
			
		}
		
		
	}
	
	
	
	
}