#include "session.h"
#include "common.h"
#include "ftp_nobody.h"
#include "ftp_proto.h"
#include "priv_sock.h"
#include "configure.h"

/* 初始化会话 */
void session_init(session_t *sess)
{
	memset(sess->command, 0, sizeof (sess->command));
	memset(sess->comm, 0, sizeof (sess->comm));
	memset(sess->args, 0, sizeof (sess->args));

	sess->ip = 0;
	memset(sess->username, 0, sizeof(sess->username));

	sess->peerfd = -1;
	sess->nobody_fd = -1;
	sess->proto_fd = -1;

	sess->user_uid = 0;
	sess->ascii_mode = 0;

	sess->p_addr = NULL;
	sess->data_fd = -1;
	sess->listen_fd = -1;

	sess->restart_pos = 0;
	sess->rnfr_name = NULL;

	sess->limits_max_upload = tunable_upload_max_rate * 1024;
	sess->limits_max_download = tunable_download_max_rate * 1024;
	sess->start_time_sec = 0;
	sess->start_time_usec = 0;

	sess->is_translating_data = 0;
	sess->is_receive_abor = 0;

	sess->curr_clients = 0;
	sess->curr_ip_clients = 0;
}

void session_reset_command(session_t *sess)
{
	memset(sess->command, 0, sizeof (sess->command));
	memset(sess->comm, 0, sizeof (sess->comm));
	memset(sess->args, 0, sizeof (sess->args));
}

void session_begin(session_t *sess)
{
    /* 创建一对UNIX域套接字，用于进程间通信 */
	priv_sock_init(sess);

	pid_t pid;
	if((pid = fork()) == -1)
		ERR_EXIT("fork");
	else if(pid == 0)
	{
        /* 子进程 */
		priv_sock_set_proto_context(sess);
		handle_proto(sess);
	}
	else
	{
		priv_sock_set_nobody_context(sess);
		handle_nobody(sess);
	}

}
