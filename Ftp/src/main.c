
#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "configure.h"
#include "parse_conf.h"
#include "ftp_assist.h"


int main(int argc, char const *argv[])

{
    /* 检查执行当前进程的真实用户ID，只有root用户才具有权限执行 */
	check_permission();

    /* 父进程捕获子进程的SIGCHLD信号，并对其进程处理 */
	setup_signal_chld();

    /* 加载并解析ftp配置文件 */
	parseconf_load_file("conf\/ftpserver.conf");
    /* 打印配置信息 */
	print_conf();

    /*
     * 初始化hash表，即维护两个hash表：
     * 1、进程ID到IP地址的hash表；
     * 2、IP地址到客户端数量的hash表；
     */
	init_hash();

	/* 创建一个端口号为 21 的监听套接字 */
	int listenfd = tcp_server(tunable_listen_address, tunable_listen_port);

	pid_t pid;
	session_t sess;
    /* 初始化会话管理结构成员 */
	session_init(&sess);
	p_sess = &sess;	/* 配置全局变量 */
	
	while(1)
	{
		struct sockaddr_in addr;
        /*
         * 在规定时间内接收客户端的连接请求，
         * 并返回已连接套接字peerfd，将客户端地址保存在addr中；
         */
		int peerfd = accept_timeout(listenfd, &addr, tunable_accept_timeout);
        /* 若接收客户端请求超时，则继续准备下一次接收 */
		if(peerfd  == -1 && errno == ETIMEDOUT)
			continue;
        /* 出错则退出 */
		else if(peerfd == -1)
			ERR_EXIT("accept_timeout");

		/* 获取客户端请求的ip地址，并将其添加到hash表中 */
		uint32_t ip = addr.sin_addr.s_addr;
        /* 设置当前会话中的ip地址 */
		sess.ip = ip;
        /* 将ip地址添加到hash表中，并更新当前会话的客户端数量 */
		add_clients_to_hash(&sess, ip);
	    /* 创建子进程 */
        if((pid = fork()) == -1)
			ERR_EXIT("fork");
		else if(pid == 0)
		{/* 在子进程中处理当前客户端请求 */
			close(listenfd);/* 关闭监听套接字 */

			sess.peerfd = peerfd;
            /* 判断是否超出当前ftp处理的客户端数量的限制 */
			limit_num_clients(&sess);
            /* 开始与客户端进行会话 */
			session_begin(&sess);
			exit(EXIT_SUCCESS);
		}
		else
		{/* 在父进程中 */
			/* 记录处理当前客户端的子进程ID对应的ip地址 */
			add_pid_ip_to_hash(pid, ip);
            /* 父进程关闭连接，继续监听 */
			close(peerfd);
		}
	}

	return 0;
}

