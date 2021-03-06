#ifndef _PT_CLIENT_INCLUED_H_
#define _PT_CLIENT_INCLUED_H_


#include <ptnetwork/buffer.h>
#include <ptnetwork/packet.h>

struct pt_client;


enum pt_client_state
{
    PT_NO_CONNECT,     //客户端没有建立与服务器的连接
    PT_CONNECTING,      //客户端正在连接服务器
    PT_CONNECTED,       //客户端连接服务器成功
	PT_CONNECT_FAILED   //与服务器建立连接失败
};



typedef void (*pt_cli_on_connected)(struct pt_client *conn, enum pt_client_state state);
typedef void (*pt_cli_on_receive)(struct pt_client *conn, struct pt_buffer *buff);
typedef void (*pt_cli_on_disconnected)(struct pt_client *conn);
typedef void (*pt_cli_on_error)(struct pt_client *conn, const char *function);
struct pt_client
{
    //socket
    pt_net_t conn;
    
    //uv_loop 主循环
    uv_loop_t *loop;
    
    //成功连接服务器后调用
    pt_cli_on_connected on_connected;
    
    //数据到达后调用
    pt_cli_on_receive on_receive;
    
    //断开连接后调用
    pt_cli_on_disconnected on_disconnected;

    //当前客户端的连接状态
    enum pt_client_state state;
    
    //收到的缓冲区数据
    struct pt_buffer *buf;
    
    //投递给libuv的异步缓冲区
    uv_buf_t *async_buf;
    
    
    //用户data
    void *data;
    
    
    //加密函数使用
    uint32_t serial;
    RC4_KEY encrypt_ctx;
    qboolean enable_encrypt;
    uint32_t encrypt_key[4];
    
    /*
     提供给libuv的回调函数
     */
    uv_write_cb write_cb;
    uv_read_cb read_cb;
    uv_connect_cb connect_cb;
    uv_shutdown_cb shutdown_cb;
	
	pt_cli_on_error on_error;
	int last_error;
};

struct pt_client *pt_client_new();
void pt_client_free(struct pt_client *client);


//初始化一个pt_client结构，设置回调函数等
void pt_client_init(uv_loop_t *loop, struct pt_client *client, pt_cli_on_connected on_connected, pt_cli_on_receive on_receive, pt_cli_on_disconnected on_disconnected);

void inline pt_client_init_v2(struct pt_client *client, uv_loop_t *loop,pt_cli_on_connected on_connected, pt_cli_on_receive on_receive, pt_cli_on_disconnected on_disconnected)
{
	pt_client_init(loop, client,on_connected, on_receive, on_disconnected);
}
//连接服务器
qboolean pt_client_connect(struct pt_client *client, const char *host, uint16_t port);
qboolean pt_client_connect_pipe(struct pt_client *client, const char *path);
void pt_client_disconnect(struct pt_client *client);

//添加发送数据到队列中
qboolean pt_client_send(struct pt_client *client, struct pt_buffer *buff);

//设置加密解密信息
void pt_client_set_encrypt(struct pt_client *client, const uint32_t encrypt_key[4]);

#endif
