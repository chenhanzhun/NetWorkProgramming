/*
 * @文件功能：
 * 检查执行ftp服务器用户的权限；
 * 维护hash表的相关操作；
 * 捕获SIGCHLD信号，并对其进行处理；
 * 打印配置文件内容；
 * 限制客户端的数量；
 */
#include "ftp_assist.h"
#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "configure.h"
#include "parse_conf.h"
#include "ftp_codes.h"
#include "command_map.h"
#include "hash.h"



/* 客户连接数量 */
unsigned int num_of_clients = 0;

hash_t *ip_to_clients;
hash_t *pid_to_ip;

static void handle_sigchld(int sig);
static unsigned int hash_func(unsigned int buckets, void *key);
/* 向维护IP地址的hash表添加新的IP地址 */
static unsigned int add_ip_to_hash(uint32_t ip);

Sigfunc *Msignal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    /* 设置信号处理函数 */
    act.sa_handler = func;
    /* 初始化信号集 */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM)
    {/* 若是SIGALRM信号，则系统不会自动重启 */
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    }
    else
    {/* 其余信号设置为系统会自动重启 */
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
    /* 调用 sigaction 函数 */
    if(sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return(oact.sa_handler);
}
/* 检查执行权限 */
void check_permission()
{
    /* 获取执行当前进程的真实用户ID */
	if(getuid())/* root执行时返回 0 */
	{
		fprintf(stderr, "FtpServer must be started by root\n");
		exit(EXIT_FAILURE);
	}
}

/* SIGCHLD信号处理函数 */
void setup_signal_chld()
{
    /*
     * 当子进程终止时会将SIGCHLD信号发送给其父进程；
     * 父进程捕获该信号，并对其进程处理；
     */
	if(Msignal(SIGCHLD, handle_sigchld) == SIG_ERR)
		ERR_EXIT("signal");
}

/* 打印配置信息 */
void print_conf()
{
	printf("tunable_pasv_enable=%d\n", tunable_pasv_enable);
    printf("tunable_port_enable=%d\n", tunable_port_enable);

    printf("tunable_listen_port=%u\n", tunable_listen_port);
    printf("tunable_max_clients=%u\n", tunable_max_clients);
    printf("tunable_max_per_ip=%u\n", tunable_max_per_ip);
    printf("tunable_accept_timeout=%u\n", tunable_accept_timeout);
    printf("tunable_connect_timeout=%u\n", tunable_connect_timeout);
    printf("tunable_idle_session_timeout=%u\n", tunable_idle_session_timeout);
    printf("tunable_data_connection_timeout=%u\n", tunable_data_connection_timeout);
    printf("tunable_local_umask=0%o\n", tunable_local_umask);
    printf("tunable_upload_max_rate=%u\n", tunable_upload_max_rate);
    printf("tunable_download_max_rate=%u\n", tunable_download_max_rate);

    if (tunable_listen_address == NULL)
        printf("tunable_listen_address=NULL\n");
    else
        printf("tunable_listen_address=%s\n", tunable_listen_address); 
}

/* 限制客户端的数量 */
void limit_num_clients(session_t *sess)
{
	if(tunable_max_clients > 0 && sess->curr_clients > tunable_max_clients)
	{
		//421 There are too many connected users, please try later.
		ftp_reply(sess, FTP_TOO_MANY_USERS, "There are too many connected users, please try later.");
		exit(EXIT_FAILURE);
	}

	if(tunable_max_per_ip > 0 && sess->curr_ip_clients > tunable_max_per_ip)
	{
		//421 There are too many connections from your internet address.
		ftp_reply(sess, FTP_IP_LIMIT, "There are too many connections from your internet address.");
		exit(EXIT_FAILURE);
	}
}

/* 父进程对子进程发起的SIGCHLD信号的处理函数 */
static void handle_sigchld(int sig)
{
	pid_t pid;
    /* -1表示等待任意子进程终止；WNOHANG终止的子进程并不立即使用，即该函数不阻塞； */
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0)
	{
        /* 子进程终止，则需减少客户连接数 */
		--num_of_clients;
		//pid -> ip
        /* 根据pid键值查找对应的原始IP地址值 */
		uint32_t *p_ip = (uint32_t*)hash_lookup_value_by_key(pid_to_ip, &pid, sizeof(pid));
		assert(p_ip != NULL); //ip必须能找到
		uint32_t ip = *p_ip;
		//ip -> clients
        /* 由ip地址查找所对应的客户端 */
		unsigned int *p_value = (unsigned int *)hash_lookup_value_by_key(ip_to_clients, &ip, sizeof(ip));
		assert(p_value != NULL && *p_value > 0);
		--*p_value;

		/* 释放PID进程所对应的hash表的节点 */
		hash_free_entry(pid_to_ip, &pid, sizeof(pid));
	}
}

/* hash函数 */
static unsigned int hash_func(unsigned int buckets, void *key)
{
    unsigned int *number = (unsigned int*)key;

    return (*number) % buckets;
}


/* 将ip添加到hash表中，并返回当前ip的客户端数量 */
static unsigned int add_ip_to_hash(uint32_t ip)
{
	unsigned int *p_value = (unsigned int *)hash_lookup_value_by_key(ip_to_clients, &ip, sizeof(ip));
	if(p_value == NULL)
	{
		unsigned int value = 1;
		hash_add_entry(ip_to_clients, &ip, sizeof(ip), &value, sizeof(value));
		return 1;
	}
	else
        return ++*p_value;
    /*
	{
		unsigned int value = *p_value;
		value++;
		*p_value  = value;

		return value;
	}
    */
}


/* 初始化hash表示 */
void init_hash()
{
    /* 建立一个buckets为256，hash函数为hash_func的哈希表 */
    /* 这里维持两个hash表：进程ID到IP地址的hash表、IP地址到客户端的hash表 */
	ip_to_clients = hash_alloc(256, hash_func);
	pid_to_ip = hash_alloc(256, hash_func);
}

/* 将客户端连接ip添加到hash表中 */
void add_clients_to_hash(session_t *sess, uint32_t ip)
{
    /* 记录已连接的客户端数量 */
	++num_of_clients;
    /* 所有ip总的客户端数量 */
	sess->curr_clients = num_of_clients;
    /* 某一个ip的客户端数量 */
	sess->curr_ip_clients = add_ip_to_hash(ip);
}

void add_pid_ip_to_hash(pid_t pid, uint32_t ip)
{
	hash_add_entry(pid_to_ip, &pid, sizeof(pid), &ip, sizeof(ip));
}
