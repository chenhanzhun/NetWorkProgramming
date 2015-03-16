#ifndef TRANS_CTRL_H
#define TRANS_CTRL_H

#include "session.h"

void limit_curr_rate(session_t *sess, int nbytes, int is_upload);

//控制连接
void setup_signal_alarm_ctrl_fd();	
void start_signal_alarm_ctrl_fd();

//数据连接
void setup_signal_alarm_data_fd();
void start_signal_alarm_data_fd();

void cancel_signal_alarm();

void setup_signal_sigurg();


void do_site_chmod(session_t *sess, char *args);
void do_site_umask(session_t *sess, char *args);
void do_site_help(session_t *sess);


#endif	/* TRANS_CTRL_H */
