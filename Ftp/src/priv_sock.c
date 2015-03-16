#include "priv_sock.h"
#include "common.h"
#include "sysutil.h"
#include "trans_ctrl.h"


void priv_sock_init(session_t *sess)
{
    int fds[2];
    /* 创建一对UNIX域的套接字，用于进程间通信 */
    if(socketpair(PF_UNIX, SOCK_STREAM, 0, fds) == -1)
        ERR_EXIT("socketpair");

    sess->nobody_fd = fds[0];
    sess->proto_fd = fds[1];
}

void priv_sock_close(session_t *sess)
{
    if(sess->nobody_fd != -1)
    {
        close(sess->nobody_fd);
        sess->nobody_fd = -1;
    }

    if(sess->proto_fd != -1)
    {
        close(sess->proto_fd);
        sess->proto_fd = -1;
    }
}
void priv_sock_set_nobody_context(session_t *sess)
{
    /* 关闭控制连接 */
    close(sess->peerfd);

    if(sess->proto_fd != -1)
    {
        /* 在父进程中关闭proto_fd套接字 */
        close(sess->proto_fd);
        sess->proto_fd = -1;
    }
}

void priv_sock_set_proto_context(session_t *sess)
{
    if(sess->nobody_fd != -1)
    {
        /* 在子进程中关闭nobody_fd套接字 */
        close(sess->nobody_fd);
        sess->nobody_fd = -1;
    }

    /* 启动控制连接定时器，捕获SIGALRM信号，并对其进行处理 */
    setup_signal_alarm_ctrl_fd();
    /* 安装SIGURG信号，并对其进行处理 */
    setup_signal_sigurg();
    /* 开启带外数据，即设置套接字选项使其支持带外数据 */
    activate_oobinline(sess->peerfd);
    /* 当有带外数据或紧急数据到来时，允许发送SIGURG信号 */
    activate_signal_sigurg(sess->peerfd);
}

void priv_sock_send_cmd(int fd, char cmd)
{
    int ret = writen(fd, &cmd, sizeof cmd);
    if(ret != sizeof(cmd))
    {
        fprintf(stderr, "priv_sock_send_cmd error\n");
        exit(EXIT_FAILURE);
    }
}

char priv_sock_recv_cmd(int fd)
{
    char res;
    /* 读取指定的sizeof res字节的数据 */
    int ret = readn(fd, &res, sizeof res);
    /* 子进程套接字已关闭，则接收到的数据ret为0 */
    if(ret == 0)
    {
        printf("Proto close!\n");
        exit(EXIT_SUCCESS);
    }
    /* 接收过程中出错 */
    if(ret != sizeof(res))
    {
        fprintf(stderr, "priv_sock_recv_cmd error\n");
        exit(EXIT_FAILURE);
    }

    /* 成功接收到命令，则返回该命令 */
    return res;
}

void priv_sock_send_result(int fd, char res)
{
    int ret = writen(fd, &res, sizeof res);
    if(ret != sizeof(res))
    {
        fprintf(stderr, "priv_sock_send_result\n");
        exit(EXIT_FAILURE);
    }
}

char priv_sock_recv_result(int fd)
{
    char res;
    int ret = readn(fd, &res, sizeof res);
    if(ret != sizeof(res))
    {
        fprintf(stderr, "priv_sock_recv_result error\n");
        exit(EXIT_FAILURE);
    }

    return res;
}

void priv_sock_send_int(int fd, int the_int)
{
    int ret = writen(fd, &the_int, sizeof(int));
    if(ret != sizeof(the_int))
    {
        fprintf(stderr, "priv_sock_send_int error\n");
        exit(EXIT_FAILURE);
    }
}

int priv_sock_recv_int(int fd)
{
    int res;
    int ret = readn(fd, &res, sizeof(res));
    if(ret != sizeof(res))
    {
        fprintf(stderr, "priv_sock_recv_int error\n");
        exit(EXIT_FAILURE);
    }

    return res;
}

void priv_sock_send_str(int fd, const char *buf, unsigned int len)
{
    priv_sock_send_int(fd, (int)len);
    int ret = writen(fd, buf, len);
    if(ret != (int)len)
    {
        fprintf(stderr, "priv_sock_send_str error\n");        
        exit(EXIT_FAILURE);
    }
}


void priv_sock_recv_str(int fd, char *buf, unsigned int len)
{
    unsigned int recv_len = (unsigned int)priv_sock_recv_int(fd);
    if (recv_len > len)
    {
        fprintf(stderr, "priv_sock_recv_str error\n");
        exit(EXIT_FAILURE);
    }

    int ret = readn(fd, buf, recv_len);
    if (ret != (int)recv_len)
    {
        fprintf(stderr, "priv_sock_recv_str error\n");
        exit(EXIT_FAILURE);
    }
}

void priv_sock_send_fd(int sock_fd, int fd)
{
    send_fd(sock_fd, fd);
}

int priv_sock_recv_fd(int sock_fd)
{
    return recv_fd(sock_fd);
}
