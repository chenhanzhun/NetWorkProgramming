#ifndef PRIV_OPS_H
#define PRIV_OPS_H 

#include "session.h"

void privop_port_get_data_sock(session_t *sess);
void privop_pasv_active(session_t *sess);
void privop_pasv_listen(session_t *sess);
void privop_pasv_accept(session_t *sess);

#endif  /*PRIV_OPS_H*/
