/* 守护进程 */

#include <syslog.h>

void syslog(int priority, const char *message, ...);

void openlog(const char *ident, int options, int facility);

void closelog(void);

/*
 * 说明：
 * 函数openlog和closelog是可选的，若不调用openlog函数，则在第一次调用syslog函数时，会自动调用openlog函数；
 * openlog可以在首次调用syslog前调用，closelog可以在应用进程不再需要发送日志消息时调用；
 * ident参数是一个由syslog冠于每个日志消息之前的字符串，通常是程序名；
 * ******************************************************************
 * options参数由以下值一个或多个逻辑“或”组成：
 * ******************************************************************
 * （1）LOG_CONS    若无法发送到syslogd守护进程，则将消息登记到控制台
 * （2）LOG_NDELAY  不延迟打开，立即创建套接字
 * （3）LOG_PERROR  既发送到syslogd守护进程，又登记到标准错误输出
 * （4）LOG_PID     每条日志消息都登记进程ID
 * （5）LOG_NOWAIT  不等待在消息记入日志过程中可能已创建的子进程
 * （6）LOG_ODELAY  在记录第一条消息之前延迟打开至syslogd守护进程的连接
 *********************************************************************
 * 参数 priority是级别（level）和设施（facility）两者的组合；
 * level和facility的目的在于允许在/dev/syslog.conf文件中统一配置来自同一个给定设施的所有消息，
 * 或统一配置具有相同级别的所有消息；
 * *********************************************************
 * 其中level取值如下：（注：以下按优先级从高到低）
 * *********************************************************
 * LOG_EMERG    系统不可使用
 * LOG_ALERT    必须立即采取修复
 * LOG_CRIT     临界条件
 * LOG_ERR      出错条件
 * LOG_WARNING  警告条件
 * LOG_NOTICE   正常然而重要的条件（默认值）
 * LOG_INFO     通告消息
 * LOG_DEBUG    调试级消息
 ***********************************************************
 ***********************************************************
 * facility取值如下：
 * *********************************************************
 * LOG_AUTH     安全/授权消息
 * LOG_AUTHPRIV 安全/授权消息（私用）
 * LOG_CRON     cron守护进程
 * LOG_DAEMON   系统守护进程
 * LOG_FTP      FTP守护进程
 * LOG_KERN     内核产生的消息
 * LOG_LOCAL0   保留由本地使用
 * LOG_LOCAL1   保留由本地使用
 * LOG_LOCAL2   保留由本地使用
 * LOG_LOCAL3   保留由本地使用
 * LOG_LOCAL4   保留由本地使用
 * LOG_LOCAL5   保留由本地使用
 * LOG_LOCAL6   保留由本地使用
 * LOG_LOCAL7   保留由本地使用
 * LOG_LPR      行打印机系统
 * LOG_MALL     邮件系统
 * LOG_NEWS     网络新闻系统
 * LOG_SYSLOG   由syslogd内部产生的消息
 * LOG_USER     任意用户级消息（默认）
 * LOG_UUCP     UUCP系统
 * *********************************************************
 */
