#ifndef TRANS_DATA_H
#define TRANS_DATA_H

#include "session.h"

void download_file(session_t *sess);
void upload_file(session_t *sess, int is_appe);
void trans_list(session_t *sess, int list);

#endif /* TRANS_DATA_H */