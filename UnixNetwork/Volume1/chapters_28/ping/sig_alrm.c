#include	"ping.h"

/* 发送数据包，并设置闹钟，一秒钟后给所在进程发送SIGALRM信号  */
void
sig_alrm(int signo)
{
	(*pr->fsend)();

	alarm(1);
	return;
}
