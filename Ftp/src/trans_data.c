/*
 * @ 文件功能：
 * 文件下载；
 * 文件上传；
 * 列出传输目录；
 */
#include "trans_data.h"
#include "common.h"
#include "sysutil.h"
#include "ftp_codes.h"
#include "command_map.h"
#include "configure.h"
#include "priv_sock.h"
#include "trans_ctrl.h"


static const char *statbuf_get_perms(struct stat *sbuf);
static const char *statbuf_get_date(struct stat *sbuf);
static const char *statbuf_get_filename(struct stat *sbuf, const char *name);
static const char *statbuf_get_user_info(struct stat *sbuf);
static const char *statbuf_get_size(struct stat *sbuf);

static int is_port_active(session_t *sess);
static int is_pasv_active(session_t *sess);

static void get_port_data_fd(session_t *sess);
static void get_pasv_data_fd(session_t *sess);

static void trans_list_common(session_t *sess, int list);
static int get_trans_data_fd(session_t *sess);


void download_file(session_t *sess)
{
    /* 为 1 表示需要进行数据传输 */
    sess->is_translating_data = 1;

    /* 获取数据连接上的套接字 */
    if(get_trans_data_fd(sess) == 0)
    {
        ftp_reply(sess, FTP_FILEFAIL, "Failed to open file."); 
        return;
    }
        
    /* 打开需要下载的文件 */
    int fd = open(sess->args, O_RDONLY);
    if(fd == -1)
    {
        ftp_reply(sess, FTP_FILEFAIL, "Failed to open file."); 
        return;
    }

    /* 对已打开的文件描述符进行加锁，使其他进程不能对其操作 */
    if(lock_file_read(fd) == -1)
    {
        ftp_reply(sess, FTP_FILEFAIL, "Failed to open file."); 
        return;
    }

    /* 判断是否是普通文件 */
    struct stat sbuf;
    /* 获取文件描述符的属性，并将其保存在sbuf中 */
    if(fstat(fd, &sbuf) == -1)
        ERR_EXIT("fstat");
    if(!S_ISREG(sbuf.st_mode))
    {
        /* 若不是普通文件 */
        ftp_reply(sess, FTP_FILEFAIL, "Can only download regular file."); 
        return;
    }

    /* 判断是否需要进行断点续传 */
    unsigned long filesize = sbuf.st_size;/* 文件剩余的字节数 */

    /* 判断上次文件断点位置 */
    int offset = sess->restart_pos;
    if(offset != 0)
    {
        filesize -= offset;
    }

    /* 定位到断点处 */
    if(lseek(fd, offset, SEEK_SET) == -1)
        ERR_EXIT("lseek");

    //150 ascii
    //150 Opening ASCII mode data connection for /home/wing/redis-stable.tar.gz (1251318 bytes).
    char text[1024] = {0};
    if(sess->ascii_mode == 1)
        snprintf(text, sizeof text, "Opening ASCII mode data connection for %s (%lu bytes).", sess->args, filesize);
    else
        snprintf(text, sizeof text, "Opening Binary mode data connection for %s (%lu bytes).", sess->args, filesize);
    ftp_reply(sess, FTP_DATACONN, text);

    /* 记录时间开始下载文件的时间 */
    sess->start_time_sec = get_curr_time_sec();
    sess->start_time_usec = get_curr_time_usec();

    /* 文件传输过程中的参数 */
    int flag = 0;   //记录下载的结果
    int nleft = filesize;   //剩余字节数
    int block_size = 0; //一次传输的字节数
    const int kSize = 65536;/* 一次最多传输的字节数 */
    while(nleft > 0)
    {
        block_size = (nleft > kSize) ? kSize : nleft;
        /* 将读取文件描述符fd的内容写入到数据连接上的套接字data_fd上 */
        int nwrite = sendfile(sess->data_fd, fd, NULL, block_size);

        if(sess->is_receive_abor == 1)
        {
            flag = 2;   /* 接收到ABOR命令，表示需要放弃正在传输的数据 */
            //426
            ftp_reply(sess, FTP_BADSENDNET, "Interupt downloading file.");
            sess->is_receive_abor = 0;
            break;
        }

        if(nwrite == -1)
        {
            flag = 1;   /* 读取文件数据发生错误 */
            break;
        }
        nleft -= nwrite;

        /* 对文件下载进行限速 */
        limit_curr_rate(sess, nwrite, 0);
    }
    if(nleft == 0)
        flag = 0;   /* 读取到完整的文件数据，并正常退出 */

    /* 对fd文件描述符进行解锁，同时关闭该文件描述符以及数据连接 */
    if(unlock_file(fd) == -1)
        ERR_EXIT("unlock_file");
    close(fd);
    close(sess->data_fd);
    sess->data_fd = -1;

    //226
    if(flag == 0)
        ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
    else if(flag == 1)
        ftp_reply(sess, FTP_BADSENDFILE, "Sendfile failed.");
    else if(flag == 2)
        ftp_reply(sess, FTP_ABOROK, "ABOR successful.");

    /* 设置控制连接的定时器 */
    setup_signal_alarm_ctrl_fd();
    sess->is_translating_data = 0;
}

void upload_file(session_t *sess, int is_appe)
{
    /* 为 1 表示需要进行数据传输 */
    sess->is_translating_data = 1;

    /* 获取数据连接上的套接字 */
    if(get_trans_data_fd(sess) == 0)
    {
        ftp_reply(sess, FTP_UPLOADFAIL, "Failed to get data fd."); 
        return;
    }
        
    /* 打开需要上传的文件 */
    int fd = open(sess->args, O_WRONLY | O_CREAT, 0666);
    if(fd == -1)
    {
        ftp_reply(sess, FTP_UPLOADFAIL, "Failed to open file."); 
        return;
    }

    /* 对文件描述符加上写入锁 */
    if(lock_file_write(fd) == -1)
    {
        ftp_reply(sess, FTP_UPLOADFAIL, "Failed to lock file."); 
        return;
    }

    /* 判断是否是普通文件 */
    struct stat sbuf;
    /* 获取fd描述符的文件属性，并将属性保存在sbuf中 */
    if(fstat(fd, &sbuf) == -1)
        ERR_EXIT("fstat");
    if(!S_ISREG(sbuf.st_mode))
    {/* 若不是普通文件 */
        ftp_reply(sess, FTP_UPLOADFAIL, "Can only upload regular file."); 
        return;
    }

    /* 判断是否需要断点续传 */
    long long offset = sess->restart_pos;
    /* 若不需要断点续传，且不需要追加到已存在文件的尾部 */
    if(!is_appe && offset == 0) //STOR
    {
        /* 指定已打开文件大小为0，可能会截断同名文件内容 */
        ftruncate(fd, 0);
    }
    /* 若需要进行断点续传，但是不需要追加到已存在的文件尾部 */
    else if(!is_appe && offset != 0) // REST + STOR
    {
        /* 指定已打开文件大小为offset，并定位到断点处 */
        ftruncate(fd, offset);  //截断offset后面的内容
        if(lseek(fd, offset, SEEK_SET) == -1)
            ERR_EXIT("lseek");
    }
    else    //APPE
    {
        /* 对文件进行扩展，并偏移到末尾进行追加 */
        if(lseek(fd, 0, SEEK_END) == -1)
            ERR_EXIT("lseek");
    }

    //150 ascii
    ftp_reply(sess, FTP_DATACONN, "OK to send.");

    /* 更新当前时间 */
    sess->start_time_sec = get_curr_time_sec();
    sess->start_time_usec = get_curr_time_usec();

    /* 上传文件数据 */
    char buf[65536] = {0};
    int flag = 0;
    while(1)
    {
        /* 从数据连接上套接字读取数据，并保存到接收缓冲区buf中 */
        int nread = read(sess->data_fd, buf, sizeof buf);

        if(sess->is_receive_abor == 1)
        {
            flag = 3;   //ABOR
            //426
            ftp_reply(sess, FTP_BADSENDNET, "Interupt uploading file.");
            sess->is_receive_abor = 0;
            break;
        }

        if(nread == -1)
        {
            if(errno == EINTR)
                continue;
            flag = 1;
            break;
        }
        else if(nread == 0)
        {
            flag = 0;
            break;
        }

        /* 将接收缓冲区的数据写入到文件fd中 */
        if(writen(fd, buf, nread) != nread)
        {
            flag = 2;
            break;
        }

        /* 执行限速功能 */
        limit_curr_rate(sess, nread, 1);
    }

    /* 对文件进行解锁，并关闭该文件描述符，同时关闭数据连接 */
    if(unlock_file(fd) == -1)
        ERR_EXIT("unlock_file");
    close(fd);
    close(sess->data_fd);
    sess->data_fd = -1;


    //226
    if(flag == 0)
        ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
    else if(flag == 1)
        ftp_reply(sess, FTP_BADSENDNET, "Reading from Network Failed.");
    else if(flag == 2)
        ftp_reply(sess, FTP_BADSENDFILE, "Writing to File Failed.");
    else
        ftp_reply(sess, FTP_ABOROK, "ABOR successful.");

    /* 启动控制连接定时器 */
    setup_signal_alarm_ctrl_fd();
    sess->is_translating_data = 0;
}

/* 列出传输目录列表 */
void trans_list(session_t *sess, int list)
{
    /* 获取数据连接上套接字 */
    if(get_trans_data_fd(sess) == 0)
        return ;

    //给出150 Here comes the directory listing.
    ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");

    /* 列出传输目录列表 */
    if(list == 1)
        trans_list_common(sess, 1);
    else
        trans_list_common(sess, 0);
    close(sess->data_fd);
    sess->data_fd = -1;

    /* 发送226 Directory send OK. */
    ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");
}

/*
 * @函数功能：获取数据连接上的套接字；
 * 返回值：成功返回1，失败返回0；
 */
static int get_trans_data_fd(session_t *sess)
{
    /* 判断ftp服务器当前处理哪种工作模式 */
    int is_port = is_port_active(sess);
    int is_pasv = is_pasv_active(sess);


    /* 若两种工作模式都未开启，则应返回425 */
    if(!is_port && !is_pasv)
    {
        ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
        return 0;
    }

    /* 若两种工作模式都开启，则断开该数据连接，即只能工作在其中一种模式 */
    if(is_port && is_pasv)
    {
        fprintf(stderr, "both of PORT and PASV are active\n");
        exit(EXIT_FAILURE);
    }

    /* 主动模式：获取主动模式下的数据连接 */
    if(is_port)
        get_port_data_fd(sess);

    /* 被动模式：获取被动模式下的数据连接 */
    if(is_pasv)
        get_pasv_data_fd(sess);

    /* 若成功获取到数据连接套接字，则继续执行以下程序 */
    /* 安装数据连接的SIGALRM信号，并对其进行处理 */
    setup_signal_alarm_data_fd();
    /* 设置数据连接的计时器，当超过指定时间时，产生SIGALRM信号 */
    start_signal_alarm_data_fd();

    return 1;
}


static const char *statbuf_get_perms(struct stat *sbuf)
{
    static char perms[] = "----------";
    mode_t mode = sbuf->st_mode;

    //文件类型
    switch(mode & S_IFMT)
    {
        case S_IFSOCK:
        perms[0] = 's';
        break;
        case S_IFLNK:
        perms[0] = 'l';
        break;
        case S_IFREG:
        perms[0] = '-';
        break;
        case S_IFBLK:
        perms[0] = 'b';
        break;
        case S_IFDIR:
        perms[0] = 'd';
        break;
        case S_IFCHR:
        perms[0] = 'c';
        break;
        case S_IFIFO:
        perms[0] = 'p';
        break;
    }

    //权限
    if(mode & S_IRUSR)
        perms[1] = 'r';
    if(mode & S_IWUSR)
        perms[2] = 'w';
    if(mode & S_IXUSR)
        perms[3] = 'x';
    if(mode & S_IRGRP)
        perms[4] = 'r';
    if(mode & S_IWGRP)
        perms[5] = 'w';
    if(mode & S_IXGRP)
        perms[6] = 'x';
    if(mode & S_IROTH)
        perms[7] = 'r';
    if(mode & S_IWOTH)
        perms[8] = 'w';
    if(mode & S_IXOTH)
        perms[9] = 'x';

    if(mode & S_ISUID)
        perms[3] = (perms[3] == 'x') ? 's' : 'S';
    if(mode & S_ISGID)
        perms[6] = (perms[6] == 'x') ? 's' : 'S';
    if(mode & S_ISVTX)
        perms[9] = (perms[9] == 'x') ? 't' : 'T';

    return perms;
}

static const char *statbuf_get_date(struct stat *sbuf)
{
    static char datebuf[1024] = {0};
    struct tm *ptm;
    time_t ct = sbuf->st_ctime;
    if((ptm = localtime(&ct)) == NULL)
        ERR_EXIT("localtime");

    const char *format = "%b %e %H:%M"; //时间格式

    if(strftime(datebuf, sizeof datebuf, format, ptm) == 0)
    {
        fprintf(stderr, "strftime error\n");
        exit(EXIT_FAILURE);
    }

    return datebuf;
}

static const char *statbuf_get_filename(struct stat *sbuf, const char *name)
{
    static char filename[1024] = {0};
    //name 处理链接名字
    if(S_ISLNK(sbuf->st_mode))
    {
        char linkfile[1024] = {0};
        if(readlink(name, linkfile, sizeof linkfile) == -1)
            ERR_EXIT("readlink");
        snprintf(filename, sizeof filename, "%s -> %s", name, linkfile);
    }else
    {
        strcpy(filename, name);
    }

    return filename;
}

static const char *statbuf_get_user_info(struct stat *sbuf)
{
    static char info[1024] = {0};
    snprintf(info, sizeof info, " %3d %8d %8d", sbuf->st_nlink, sbuf->st_uid, sbuf->st_gid);

    return info;
}

static const char *statbuf_get_size(struct stat *sbuf)
{
    static char buf[100] = {0};
    snprintf(buf, sizeof buf, "%8lu", (unsigned long)sbuf->st_size);
    return buf;
}


static int is_port_active(session_t *sess)
{
    return (sess->p_addr != NULL);
}

static int is_pasv_active(session_t *sess)
{
    //首先给nobody发命令
    priv_sock_send_cmd(sess->proto_fd, PRIV_SOCK_PASV_ACTIVE);
    //接收结果
    return priv_sock_recv_int(sess->proto_fd);
}


static void get_port_data_fd(session_t *sess)
{
    /* 发送cmd命令：PRIV_SOCK_GET_DATA_SOCK表示期望获取数据连接 */
    priv_sock_send_cmd(sess->proto_fd, PRIV_SOCK_GET_DATA_SOCK);
    /* 发送本地ip地址及其端口号port，以便ftp服务器与该端口建立数据连接 */
    char *ip = inet_ntoa(sess->p_addr->sin_addr);
    uint16_t port = ntohs(sess->p_addr->sin_port);
    priv_sock_send_str(sess->proto_fd, ip, strlen(ip));
    priv_sock_send_int(sess->proto_fd, port);
    /* 接收来自ftp服务器的应答 */
    char result = priv_sock_recv_result(sess->proto_fd);
    /* 建立数据连接失败 */
    if(result == PRIV_SOCK_RESULT_BAD)
    {
        ftp_reply(sess, FTP_BADCMD, "get port data_fd error");
        fprintf(stderr, "get data fd error\n");
        exit(EXIT_FAILURE);
    }
    /* 若成功建立数据连接，则获取该数据连接的套接字 */
    sess->data_fd = priv_sock_recv_fd(sess->proto_fd);

    /* 释放port模式 */
    free(sess->p_addr);
    sess->p_addr = NULL;
}

static void get_pasv_data_fd(session_t *sess)
{
    /* 给nobody发命令表示需要建立一个数据连接 */
    priv_sock_send_cmd(sess->proto_fd, PRIV_SOCK_PASV_ACCEPT);

    /* 接收nobody的响应 */
    char res = priv_sock_recv_result(sess->proto_fd);
    if(res == PRIV_SOCK_RESULT_BAD)
    {
        ftp_reply(sess, FTP_BADCMD, "get pasv data_fd error");
        fprintf(stderr, "get data fd error\n");
        exit(EXIT_FAILURE);
    }

    /* 成功返回数据连接套接字 */
    sess->data_fd = priv_sock_recv_fd(sess->proto_fd);
}

static void trans_list_common(session_t *sess, int list)
{
    /* 打开当前目录 */
    DIR *dir = opendir(".");
    if(dir == NULL)
        ERR_EXIT("opendir");

    struct dirent *dr;
    /* 读取目录流 */
    while((dr = readdir(dir)))
    {
        const char *filename = dr->d_name;
        /* 忽略“.”目录 */
        if(filename[0] == '.')
            continue;

        char buf[1024] = {0};
        struct stat sbuf;
        /* 类似于stat函数，但是可以返回符号连接的信息结构 */
        if(lstat(filename, &sbuf) == -1)
            ERR_EXIT("lstat");

        if(list == 1) // LIST
        {
            strcpy(buf, statbuf_get_perms(&sbuf));
            strcat(buf, " ");
            strcat(buf, statbuf_get_user_info(&sbuf));
            strcat(buf, " ");
            strcat(buf, statbuf_get_size(&sbuf));
            strcat(buf, " ");
            strcat(buf, statbuf_get_date(&sbuf));
            strcat(buf, " ");
            strcat(buf, statbuf_get_filename(&sbuf, filename));
        }
        else    //NLST
        {
            strcpy(buf, statbuf_get_filename(&sbuf, filename));
        }

        strcat(buf, "\r\n");
        writen(sess->data_fd, buf, strlen(buf));
    }

    closedir(dir);
}

