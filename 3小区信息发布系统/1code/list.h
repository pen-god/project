#include "wrap.h"
#include "kernel_list.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct node
{
	bool get_name;//判断是否已经获取用户名字
	int chat_socket;//聊天对象的文件描述符
	bool chat ;//聊天状态
	char chat_name[50];//聊天对象的账号
	char name[100];  
	int  new_socket; //自己的文件描述符
	struct sockaddr_in  addr;

	//镶嵌小结构体
	struct list_head list;
}listnode,*linklist;

linklist init_list()
{
	//申请头结点
	linklist head = malloc(sizeof(listnode));
	if(head!=NULL)
	{
		INIT_LIST_HEAD(&head->list);	
	}
	return head;
}

linklist new_node(bool chat)
{
	//申请新结点
	linklist p = malloc(sizeof(listnode));
	if(p !=NULL)
	{
		INIT_LIST_HEAD(&(p->list));	
	}
	p->chat=chat;
	return p;
}

void show(linklist head)
{
    struct list_head *pos;
    list_for_each(pos, &head->list)
    {
        linklist p;
        p = list_entry(pos, struct node, list);
        printf("name:%s\tnew_socket:%d\tip:%s\n",p->name,p->new_socket,inet_ntoa(p->addr.sin_addr));
		printf("\n");
    }
    
}
