/*
 * @文件功能：
 * 在port和pasv工作模式下，建立数据连接；
 */
#include "priv_ops.h"
#include "common.h"
#include "sysutil.h"
#include "priv_sock.h"
#include "configure.h"

/* 获取数据连接上的套接字 */
void privop_port_get_data_sock(session_t *sess)
{
	char ip[16] = {0};
    /* 获取客户的ip地址及其端口信息 */
	priv_sock_recv_str(sess->nobody_fd, ip, sizeof ip);
	uint16_t port = priv_sock_recv_int(sess->nobody_fd);
	/* 创建端口号为20的数据连接上的套接字 */
	int data_fd = tcp_client(20);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
    /* ftp服务器与客户端发起建立数据连接请求 */
	int ret = connect_timeout(data_fd, &addr, tunable_connect_timeout);
	if(ret == -1)
		ERR_EXIT("connect_timeout");
	priv_sock_send_result(sess->nobody_fd, PRIV_SOCK_RESULT_OK);
	priv_sock_send_fd(sess->nobody_fd, data_fd);
	close(data_fd);
}

/* 判断pasv模式是否开启 */
void privop_pasv_active(session_t *sess)
{
	/* 若listen_fd不为-1，表示已开启pasv模式，并通知 proto；*/
	priv_sock_send_int(sess->nobody_fd, (sess->listen_fd != -1));
}

/* 获取监听fd */
void privop_pasv_listen(session_t *sess)
{
	/* 创建listen fd */
	char ip[16] = {0};
	get_local_ip(ip);
    /*
	int listenfd = tcp_server(ip, 20);
    */
    /* 这里端口号为0，表示需要内核自己选定一个合适的端口号 */
	int listenfd = tcp_server(ip, 0);
	sess->listen_fd = listenfd;

	struct sockaddr_in addr;
	socklen_t len = sizeof addr;
    /* 获取listendf绑定的ip地址及其端口号，以便通知客户端 */
	if(getsockname(listenfd, (struct sockaddr*)&addr, &len) == -1)
		ERR_EXIT("getsockname");

	/* 在控制连接上告知客户端，ftp服务器已经处于监听状态 */
	priv_sock_send_result(sess->nobody_fd, PRIV_SOCK_RESULT_OK);

	/* 将ftp服务器的端口号port 告知客户端，以便客户端建立数据连接 */
	uint16_t net_endian_port = ntohs(addr.sin_port);
	priv_sock_send_int(sess->nobody_fd, net_endian_port);
}

/* accept一个新的连接 */
void privop_pasv_accept(session_t *sess)
{
	/* 接受来自客户端新的数据连接请求 */
	int peerfd = accept_timeout(sess->listen_fd, NULL, tunable_accept_timeout);
	//清除状态
	close(sess->listen_fd);
	sess->listen_fd = -1;
    if(peerfd == -1)
    {
    	priv_sock_send_result(sess->nobody_fd, PRIV_SOCK_RESULT_BAD);
        ERR_EXIT("accept_timeout");
    }
    /* 响应对方，表示成功 */
    priv_sock_send_result(sess->nobody_fd, PRIV_SOCK_RESULT_OK);

    /* 将data fd传给对方 */
    priv_sock_send_fd(sess->nobody_fd, peerfd);
    close(peerfd);
}
