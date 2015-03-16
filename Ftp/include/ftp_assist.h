#ifndef FTP_ASSIST_H
#define FTP_ASSIST_H

#include "session.h"
#include "hash.h"

extern session_t *p_sess;
extern unsigned int num_of_clients;
extern hash_t *ip_to_clients;
extern hash_t *pid_to_ip;

void init_hash();
void check_permission();
void setup_signal_chld();
void print_conf();


void limit_num_clients(session_t *sess);
void add_clients_to_hash(session_t *sess, uint32_t ip);
void add_pid_ip_to_hash(pid_t pid, uint32_t ip);

#endif /* FTP_ASSIST_H */
