#ifndef COMMAND_MAP_H
#define COMMAND_MAP_H

#include "session.h"

void do_command_map(session_t *sess);
void ftp_reply(session_t *sess, int status, const char *text);
void ftp_lreply(session_t *sess, int status, const char *text);

void do_user(session_t *sess);
void do_pass(session_t *sess);
void do_cwd(session_t *sess);
void do_cdup(session_t *sess);
void do_quit(session_t *sess);
void do_port(session_t *sess);
void do_pasv(session_t *sess);
void do_type(session_t *sess);
void do_stru(session_t *sess);
void do_mode(session_t *sess);
void do_retr(session_t *sess);
void do_stor(session_t *sess);
void do_appe(session_t *sess);
void do_list(session_t *sess);
void do_nlst(session_t *sess);
void do_rest(session_t *sess);
void do_abor(session_t *sess);
void do_pwd(session_t *sess);
void do_mkd(session_t *sess);
void do_rmd(session_t *sess);
void do_dele(session_t *sess);
void do_rnfr(session_t *sess);
void do_rnto(session_t *sess);
void do_site(session_t *sess);
void do_syst(session_t *sess);
void do_feat(session_t *sess);
void do_size(session_t *sess);
void do_stat(session_t *sess);
void do_noop(session_t *sess);
void do_help(session_t *sess);

#endif
