/*
 * @文件功能：
 * 限速传输数据；
 * 超时连接管理；
 * 处理带外数据；
 * 修改文件权限；
 * 创建文件屏蔽字；
 */
#include "trans_ctrl.h"
#include "common.h"
#include "sysutil.h"
#include "configure.h"
#include "command_map.h"
#include "ftp_codes.h"
#include "strutil.h"

session_t *p_sess = NULL;

static void handle_signal_alarm_ctrl_fd(int sig);
static void handle_signal_alarm_data_fd(int sig);
static void handle_signal_sigurg(int sig);
extern Sigfunc *Msignal(int, Sigfunc);

/* 限速功能 */
void limit_curr_rate(session_t *sess, int nbytes, int is_upload)
{
    /* 获取当前时间 */
    int curr_time_sec = get_curr_time_sec();
    int curr_time_usec = get_curr_time_usec();

    /* 求开始传输数据到当前时间的时间差 */
    double elapsed = 0.0;
    elapsed += (curr_time_sec - sess->start_time_sec);
    elapsed += (curr_time_usec - sess->start_time_usec) / (double)1000000;
    if(elapsed < 0.000001)    //double和0不能用==
        elapsed = 0.001;

    /* 求速度：已接收的字节数除以所使用的时间 */
    double curr_rate = nbytes / elapsed;

    /* 求比率 */
    double rate_radio = 0.0;
    if(is_upload)/* 对上传文件进行限速 */
    {
        /* 若用户配置了文件上传限速功能，并且当前速度已经超过了限定速度 */
        if(sess->limits_max_upload > 0 && curr_rate > sess->limits_max_upload)
        {
            rate_radio = curr_rate / (sess->limits_max_upload);
        }
        else
        {
            /* 若没有限速，必须更新时间 */
            sess->start_time_sec = get_curr_time_sec();
            sess->start_time_usec = get_curr_time_usec();
            return;
        }
    }else/* 对下载文件时进行限速 */
    {
        /* 若用户配置了文件下载限速功能，并且当前速度已经超过了限定速度 */
        if(sess->limits_max_download > 0 && curr_rate > sess->limits_max_download)
        {
            rate_radio = curr_rate / (sess->limits_max_download);
        }
        else
        {
            /* 若不限速，必须更新时间 */
            sess->start_time_sec = get_curr_time_sec();
            sess->start_time_usec = get_curr_time_usec();
            return;
        }
    }

    /* 求出需要进行限速的时间 */
    double sleep_time = (rate_radio - 1) * elapsed;

    /* 限速睡眠，实际上是让当前进程在指定时间内暂停工作 */
    if(nano_sleep(sleep_time) == -1)
        ERR_EXIT("nano_sleep");

    /* 注意更新当前时间 */
    sess->start_time_sec = get_curr_time_sec();
    sess->start_time_usec = get_curr_time_usec();
}


/* 启动控制连接的定时器，并处理其产生的SIGARLM信号 */
void setup_signal_alarm_ctrl_fd()
{
    if(Msignal(SIGALRM, handle_signal_alarm_ctrl_fd) == SIG_ERR)
        ERR_EXIT("signal");
}

/* 设置控制连接的计时器 */
void start_signal_alarm_ctrl_fd()
{
    /* 当超过指定的时间时，会产生SIGALRM信号 */
    alarm(tunable_idle_session_timeout);
}

/* 控制连接处理SIGALRM信号的函数 */
static void handle_signal_alarm_ctrl_fd(int sig)
{
    /* 若用户配置了该选项，则直接关闭控制连接 */
    if(tunable_idle_session_timeout > 0)
    {
        /* 首先关闭控制连接的读取端 */
        shutdown(p_sess->peerfd, SHUT_RD);
        //421
        ftp_reply(p_sess, FTP_IDLE_TIMEOUT, "Timeout.");
        /* 最后关闭控制连接的写入端 */
        shutdown(p_sess->peerfd, SHUT_WR);
        /* 退出 */
        exit(EXIT_SUCCESS);
    }
}

/* 启动数据连接的定时器，并处理其产生的SIGALRM信号 */
void setup_signal_alarm_data_fd()
{
    if(Msignal(SIGALRM, handle_signal_alarm_data_fd) == SIG_ERR)
        ERR_EXIT("signal");
}

/* 设置数据连接的计时器 */
void start_signal_alarm_data_fd()
{
    /* 当超过指定的时间时，会产生SIGALRM信号 */
    alarm(tunable_data_connection_timeout);
}

/* 数据连接处理SIGALRM信号的函数 */
static void handle_signal_alarm_data_fd(int sig)
{
    if(tunable_data_connection_timeout > 0)
    {
        if(p_sess->is_translating_data == 1)
        {
            /*正在传输数据，则重新启动数据连接的定时器 */
            start_signal_alarm_data_fd();
        }
        else
        {
            /* 若数据连接上没有进行数据传输，则给421，并且退出 */
            close(p_sess->data_fd);
            shutdown(p_sess->peerfd, SHUT_RD);
            ftp_reply(p_sess, FTP_DATA_TIMEOUT, "Timeout.");
            shutdown(p_sess->peerfd, SHUT_WR);
            exit(EXIT_SUCCESS);
        }
    }
}

/* 取消定时器 */
void cancel_signal_alarm()
{
    alarm(0);
}

/* 处理SIGURG信号，实质是处理带外数据 */
static void handle_signal_sigurg(int sig)
{
    char cmdline[1024] = {0};
    /* 读取命令 */
    int ret = readline(p_sess->peerfd, cmdline, sizeof cmdline);
    if(ret <= 0) //存在带外数据，不可能为0
        ERR_EXIT("readline");

    /* 去除所读取命令的\r\n字符，并将其转换为大写字符 */
    str_trim_crlf(cmdline);
    str_upper(cmdline);

    /* 判断字符是否与ABOR相关 */
    if(strcmp("ABOR", cmdline) == 0 
        || strcmp("\377\364\377\362ABOR", cmdline) == 0)
    {
        /*
         * 若确认客户端发送的是ABOR命令，表示需要强制终止当前的数据传输；
         * 则开始处理abor指令；设置标志位，并关闭数据连接，从而终止数据的传输；
         */
        p_sess->is_receive_abor = 1;
        close(p_sess->data_fd);
        p_sess->data_fd = -1;
    }
    else
    {
        //未识别的命令
        ftp_reply(p_sess, FTP_BADCMD, "Unknown command.");
    }
}

/* 安装SIGURG信号，并对该信号进行处理 */
void setup_signal_sigurg()
{
    if(Msignal(SIGURG, handle_signal_sigurg) == SIG_ERR)
        ERR_EXIT("signal");
}

/* 修改文件的权限属性 */
void do_site_chmod(session_t *sess, char *args)
{
    if (strlen(args) == 0)
    {
        ftp_reply(sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
        return;
    }

    char perm[100] = {0};
    char file[100] = {0};
    str_split(args , perm, file, ' ');
    if (strlen(file) == 0)
    {
        ftp_reply(sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
        return;
    }

    /* 八进制字符转换为无符号整数 */
    unsigned int mode = str_octal_to_uint(perm);
    /* 把文件修改为指定的权限 */
    if (chmod(file, mode) < 0)
    {
        ftp_reply(sess, FTP_CHMODOK, "SITE CHMOD command failed.");
    }
    else
    {
        ftp_reply(sess, FTP_CHMODOK, "SITE CHMOD command ok.");
    }
}
/* 创建文件的屏蔽字 */
void do_site_umask(session_t *sess, char *args)
{
    // SITE UMASK [umask]
    if (strlen(args) == 0)
    {
        char text[1024] = {0};
        sprintf(text, "Your current UMASK is 0%o", tunable_local_umask);
        ftp_reply(sess, FTP_UMASKOK, text);
    }
    else
    {
        unsigned int um = str_octal_to_uint(args);
        umask(um);
        char text[1024] = {0};
        sprintf(text, "UMASK set to 0%o", um);
        ftp_reply(sess, FTP_UMASKOK, text);
    }
}

void do_site_help(session_t *sess)
{
    //214 CHMOD UMASK HELP
    ftp_reply(sess, FTP_HELP, "CHMOD UMASK HELP");
}
