#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/bufferevent.h>

#define LISTEN_PORT 9999
#define LISTEN_BACKLOG 32

void do_accept(evutil_socket_t listener,short event,void *arg);

void read_cb(struct bufferevent *bev,void *arg);

void error_cb(struct bufferevent* bev,short event,void *arg);

void write_cb(struct bufferevent *bev,void *arg);

int main(int argc,char **argv)
{
    int ret;
    //监听套接字
    evutil_socket_t  listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    //设置socket属性
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(LISTEN_PORT);

    if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(listener, LISTEN_BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    printf ("Listening...\n");

    //设置listener为非阻塞
    evutil_make_socket_nonblocking(listener);

    //创建事件集
    struct event_base *base=event_base_new();

    //创建事件
    struct event *listen_event;
    listen_event=event_new(base,listener,EV_READ|EV_PERSIST,do_accept,(void*)base);
    //添加事件
    //添加事件，这个函数中肯定调用了创建事件时，传入的第一个参数
    event_add(listen_event,NULL);
    event_base_dispatch(base);

    return 0;
}

//这是监听线程调用的回调函数
//每次调用时肯定是有一个新的连接了所以就要把新连接的文件描述符给
//添加到监听事件中，这个事件的回调函数就变成读写的了
void do_accept(evutil_socket_t listener,short event,void *arg)
{
    struct event_base *base=(struct event_base*)arg;
    evutil_socket_t fd;

    struct sockaddr_in sin;
    socklen_t slen=sizeof(sin);

    fd=accept(listener,(struct sockaddr*)&sin,&slen);
    
    if (fd < 0) {
	perror("accept");
        return;
    }
    if (fd > FD_SETSIZE) {
        perror("fd > FD_SETSIZE\n");
        return;
    }
    printf("ACCEPT: fd = %u\n", fd);
    //io buffe事件，这个也是一个事件，只是这个事件比event特殊性
    struct bufferevent *bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);


    //设置读写对应的回调函数
    /*
     * void bufferevent_setcb(struct bufferevent *bufev,  
     *                        bufferevent_data_cb readcb, 
     *                        bufferevent_data_cb writecb,  
     *                        bufferevent_event_cb eventcb, 
     *                        void *cbarg) 
     * */



    bufferevent_setcb(bev,read_cb,NULL,error_cb,arg);

    //启用bufferevent
    bufferevent_enable(bev,EV_READ|EV_WRITE|EV_PERSIST);
}

void read_cb(struct bufferevent *bev,void *arg)
{
#define MAX_LINE 256
    char line[MAX_LINE];
    int n;
    evutil_socket_t fd=bufferevent_getfd(bev);

    while (n = bufferevent_read(bev, line, MAX_LINE), n > 0) {
        line[n] = '\0';
        printf("fd=%u, read line: %s\n", fd, line);
        bufferevent_write(bev, line, n);
    }

}

void error_cb(struct bufferevent* bev,short event,void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n"); //if bufferevent_set_timeouts() called
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
    }
    bufferevent_free(bev);
}
void write_cb(struct bufferevent *bev,void *arg){}


