//event http客户端程序

#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/event.h>  
#include <event2/bufferevent.h>  
#include <event2/buffer.h>
#include "string.h"
#include <string>

//回调函数
void http_request_done(struct evhttp_request *req, void *arg)
{
    printf("send request ok...\n");
    size_t len = evbuffer_get_length(req->input_buffer);
    unsigned char * str = evbuffer_pullup(req->input_buffer, len);
    char buf[256] = { 0 };
    memcpy(buf, str, len);

    if (str==NULL) {
        printf("len=%d\n", len);
    } else {
        printf("len = %d, str = %s\n", len, buf);
    }

    event_base_loopbreak((struct event_base*)arg);
}



void doRequest()
{

    struct event_base *base;
    struct evhttp_connection* conn;  
    struct evhttp_request* req;  
    base = event_base_new(); 
    conn = evhttp_connection_base_new(base,NULL,"127.0.0.1", 8000);  
    req = evhttp_request_new(http_request_done, base);
    //设置头信息
    evhttp_add_header(req->output_headers, "Host", "localhost");  
    //请求方法是get
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/http.js");
    evhttp_connection_set_timeout(req->evcon, 600);  
    event_base_dispatch(base);
    evhttp_connection_free(conn);  
    event_base_free(base);  
    printf("run over...\n");  
    return ;
}


void doPost()
{
    struct event_base *base;
    struct evhttp_connection* conn;  
    struct evhttp_request* req;  
    std::string strResult="{\"id\" : 123, \"name\" : \"mxd\"}";
    base = event_base_new(); 
    conn = evhttp_connection_base_new(base,NULL,"127.0.0.1", 8000);  
    req = evhttp_request_new(http_request_done, base);
    //设置头信息
    evhttp_add_header(req->output_headers, "Host", "localhost");  
    evhttp_add_header(req->output_headers, "Connection", "close");
    evhttp_add_header(req->output_headers, "Accept", "text/html");
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain;charset=utf-8");
    evbuffer_add(req->output_buffer, strResult.c_str(), strResult.length());


    //请求方法是post
    evhttp_make_request(conn, req, EVHTTP_REQ_POST, "/hello");
    evhttp_connection_set_timeout(req->evcon, 600); 
    
    event_base_dispatch(base);
    evhttp_connection_free(conn);  
    event_base_free(base);  
    printf("run over...\n");  
    return ;
}



int main()  
{
    doPost();
    return 0;
}
