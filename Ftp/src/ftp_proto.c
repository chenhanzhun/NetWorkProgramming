/*
 * @文件功能：
 * 处理来自客户端的ftp命令；
 */
#include "ftp_proto.h"
#include "common.h"
#include "sysutil.h"
#include "strutil.h"
#include "ftp_codes.h"
#include "command_map.h"
#include "trans_ctrl.h"


/* 子进程接收来自FTP客户端的FTP指令，并给予响应 */
void handle_proto(session_t *sess)
{
    /* 给予文字应答 */
	ftp_reply(sess, FTP_GREET, "(FtpServer 1.0)");
	while(1)
	{
        /* 清空命令缓冲区，准备接收来自客户端的ftp命令 */
		session_reset_command(sess);
		
		/* 设置控制连接计时器 */
		start_signal_alarm_ctrl_fd();

		/* 在控制连接套接字上接收来自客户端的ftp命令，注意：是一行一行接收 */
		int ret = readline(sess->peerfd, sess->command, MAX_COMMAND);
		if(ret == -1)
		{
			if(errno == EINTR)
				continue;
			ERR_EXIT("readline");
		}
		else if(ret == 0)
		{
			exit(EXIT_SUCCESS);
		}
        /* 去除命令行的\r与\n字符 */
		str_trim_crlf(sess->command);
        /* 将命令行名称和命名参数分开，以空格为分界线 */
		str_split(sess->command, sess->comm, sess->args, ' ');
        /* 统一将命名名称转换为大写字符 */
		str_upper(sess->comm);
        /* 打印客户端所发出的ftp命令 */
		printf("COMMD=[%s], ARGS=[%s]\n", sess->comm, sess->args);

        /* 根据ftp命令执行相应的处理函数 */
		do_command_map(sess);
	}
}



