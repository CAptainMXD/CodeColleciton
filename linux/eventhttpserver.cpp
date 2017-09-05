//event http客户端程序

#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/event.h>  
#include <event2/bufferevent.h>  
#include <event2/buffer.h>
#include "string.h"
#include <string>

void generic_cb(struct evhttp_request* req, void* arg)  
{  
    char* s = "This is the generic buf";  
    evbuffer_add(req->output_buffer, s, strlen(s));  
    evhttp_send_reply(req, 200, "OK", NULL);  
}  

void test_cb(struct evhttp_request* req, void* arg)  
{  
    char* s = "This is the test buf";  
    evbuffer_add(req->output_buffer, s, strlen(s));  
    evhttp_send_reply(req, 200, "OK", NULL);  
}  

int main()
{
    short http_port=8000;
    char *http_addr= "0.0.0.0";

    struct event_base* base = event_base_new();  
    struct evhttp* http_server = evhttp_new(base);  
    if(NULL == http_server)  
    {  
        return -1;  
    }  

    int ret = evhttp_bind_socket(http_server, http_addr, http_port);  
    if(ret != 0)  
    {  
        return -1;  
    }  

    evhttp_set_cb(http_server, "/test", test_cb,NULL);
    evhttp_set_gencb(http_server,generic_cb,NULL);  

    printf("http server start OK!\n");  
    event_base_dispatch(base);
    evhttp_free(http_server);
    return 0;
}









