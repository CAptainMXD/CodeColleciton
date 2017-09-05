#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h> 
#include <fcntl.h> 


#define MAXEPOLLSIZE 10000
#define MAXLINE 1024
#define MAXFD 1024 

typedef int socket_t;


//线程B是一个客户端线程

//公用缓冲区用来存储已经accept的套结字，
//其中一个线程放套接字，另一个线程取套接字
//由于是两个线程所以要用锁进行保护
int fdsbuffer[MAXFD];


int isConn=-1;//用来表示是否连接了服务器B



//接收客户端发来的数据
//并且线程A把数据放到这边，线程B来读,并发送到服务器上
int sendbuffer[MAXLINE];
pthread_mutex_t *sendbufferlock;


//线程B接收服务器B发来的数据，并且由线程A读取并发送到客户端
int recvbuffer[MAXLINE];
pthread_mutex_t *recvbufferlock;



//对一个互斥锁
pthread_mutex_t *lock;
//这两个变量是通信线程的数量
int epollfd;
struct epoll_event events[MAXEPOLLSIZE];



//初始化缓冲区
void ini_buffer()
{
	memset(&recvbuffer,0,sizeof(recvbuffer));
	memset(&sendbuffer,0,sizeof(sendbuffer));
	int i=0;
	for(i=0;i<MAXFD;i++)
	{
		fdsbuffer[i]=-1;
	}
}


//初始化锁
void ini_lock()
{
	lock=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock, NULL);
	
	recvbufferlock=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(recvbufferlock, NULL);
	
	sendbufferlock=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(sendbufferlock, NULL);


}

//释放锁
void del_lock()
{
	pthread_mutex_destroy((pthread_mutex_t*)recvbufferlock);
	free(recvbufferlock);
	
	pthread_mutex_destroy((pthread_mutex_t*)sendbufferlock);
	free(sendbufferlock);
	
	pthread_mutex_destroy((pthread_mutex_t*)lock);
	free(lock);
}


//对缓冲区上锁
void enter_lock(void * lock)
{
	pthread_mutex_lock((pthread_mutex_t*)lock);
}


//释放缓冲区的锁
void leave_lock(void * lock)
{
	pthread_mutex_unlock((pthread_mutex_t*)lock);
}


//关闭套接字
void socket_close(socket_t  s )
{
	if (s != -1)
	{
		close(s);
	}
}


//设置非阻塞
int socket_set_nonblocking(socket_t s)
{
	int ret=fcntl(s, F_SETFL, O_NONBLOCK);
	return ret;
}

//根据ip和port创建一个非阻塞监听socket
socket_t socket_listen( const char * ip, int port, int backlog )
{
	socket_t s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		return -1;
	}

	// bind
	struct sockaddr_in _sockaddr;
	memset(&_sockaddr, 0, sizeof(_sockaddr));
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sockaddr.sin_port = htons(port);

	int ret =bind(s, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr));
	if (ret != 0)
	{
		socket_close(s);
		return -1;
	}

	// listen
	ret = listen(s, backlog);
	if (ret != 0)
	{
		socket_close(s);
		return -1;
	}
	return s;
}


//当新的连接来时来返回一个监听的socket
socket_t socket_accept( socket_t s)
{
	// 新连接
	struct sockaddr_in _sockaddr;
	memset(&_sockaddr, 0, sizeof(_sockaddr));
	socklen_t size = sizeof(_sockaddr);
	socket_t news = accept(s, (struct sockaddr*)&_sockaddr, &size);
	if (news == -1)
	{
		return -1;
	}
	socket_set_nonblocking(news);
	//printf("news:%d \n",news);
	return news;
}


//把这个listenfd放入到缓冲区中
void write_socket_to_buffer(int listenfd)
{
	//printf("write_socket_to_buffer\n");
	int i=0;
	enter_lock(lock);	
	for(i=0;i<MAXFD;i++)
	{
	     if(fdsbuffer[i]!=-1) continue;
	     fdsbuffer[i]=listenfd; break;	
	}
	leave_lock(lock);
}

//将可用的socket并添加到监听事件集中
void addSocket(int fd)
{
	if(fd==-1) return ;
        struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.ptr = NULL;
	ev.data.fd=fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	return ;
}

void readAndAddSocket()
{
	int i;
	enter_lock(lock);
	for(i=0;i<MAXFD;i++)
	{
	     if(fdsbuffer[i]!=-1){
		addSocket(fdsbuffer[i]);
		fdsbuffer[i]=-1;
	     }
	     else continue;
	}
	leave_lock(lock);
}


int handle(int connfd)
{

    int nread;
    memset(buf,0,sizeof(buf));
   
    nread = read(connfd, buf, MAXLINE);//读取客户端socket流
    if (nread == 0) {
        printf("client close the connection\n");
        close(connfd);
        return -1;
    } 
    if (nread < 0) {
        perror("read error");
        close(connfd);
        return -1;
    }
 
    //把数据写入到缓冲区 
    enter_lock(sendbufferlock);   
    write(connfd, sendbuf, nread);//响应客户端  
    leave_lock(sendbufferlock);
    return 0;
}



int  Select()
{	int i=0;;
	//返回就绪的个数
	int ret = epoll_wait(epollfd, events, MAXFD, 5);
	if (ret < 0)
	{
		if(EINTR == errno)
		{
			return 0;
		}
		return -1;
	}

	for(i = 0; i < ret; i++)
	{
		socket_t s = events[i].data.fd;
		//printf("ready socket=%d\n",s);
		
		int e = events[i].events;
		if (e & EPOLLIN)
		{
			
			//printf("ret=%d acceptfd=%d: \n",ret,s);
			handle(s);
		}
		if (e & EPOLLOUT)
		{

		}
	}
      return 0;

}

void *pthreadAProc(void *param)
{
	while(1)
	{
		readAndAddSocket();
		Select();
		sleep(1);
	}
}

void readsendBuffer(char *buf)
{
    //从缓冲区读数据 
    enter_lock(sendbufferlock);   

    memcpy(buf, sendbuf, sizeof(sendbuf)); 
    memset(&sendbuf,0,sizeof(sendbuf));

    leave_lock(sendbufferlock);
}

socket_t conn()
{
	socket_t s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		return s;
	}	

	// connect
	struct sockaddr_in _sockaddr;
	memset(&_sockaddr, 0, sizeof(_sockaddr));
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(9999);
	_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = connect(s, (struct  sockaddr *)&_sockaddr, sizeof(_sockaddr));
	if (ret != 0)
	{
		socket_close(s);
		return -1;
	}
	
	isConn=0;
	return s;
}


//线程B是一个客户端，先连接服务器B，然后不断地读写sendBuffer
void *pthreadBProc(void *param)
{
	char buf[MAXLINE];
	while(1)
	{
		if(isConn==-1)
			s=conn();

		memset(&buf,0,sizeof(buf));
		//读出数据后把数据发送到另一个服务器B上
		readsendBuffer(buf);
		
		write(s,buf,sizeof(buf));
		
		sleep(1);
	}
}



int main()
{
	ini_buffer();
	ini_lock(lock);
	epollfd=epoll_create(MAXEPOLLSIZE);
	
	int listenfd=socket_listen("127.0.0.1",8888,25);
	if(listenfd<0)return;	

	pthread_t tmp; 
	
	pthread_create(&tmp, NULL,pthreadProc, NULL);	
	
	for(;;)
	{
		int accfd=socket_accept(listenfd);
		write_socket_to_buffer(accfd);
		sleep(1);
	}
	
        
	socket_close(listenfd);
	pthread_join(tmp);
	
	return 0;
}



