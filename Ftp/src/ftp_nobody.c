#include "ftp_nobody.h"
#include "common.h"
#include "sysutil.h"
#include "priv_ops.h"
#include "priv_sock.h"

void set_nobody();
void set_bind_capabilities();
int capset(cap_user_header_t hdrp, const cap_user_data_t datap);

/* nobody时刻准备接收来自子进程的命令 */
void handle_nobody(session_t *sess)
{
    /* 设置帐号nobody为当前进程的有效用户 */
    set_nobody();
    /* 添加绑定端口号20的特权 */
    set_bind_capabilities();

    char cmd;
    while(1)
    {
        cmd = priv_sock_recv_cmd(sess->nobody_fd);
        switch (cmd)
        {
            case PRIV_SOCK_GET_DATA_SOCK:
                privop_port_get_data_sock(sess);
                break;
            case PRIV_SOCK_PASV_ACTIVE:
                privop_pasv_active(sess);
                break;
            case PRIV_SOCK_PASV_LISTEN:
                privop_pasv_listen(sess);
                break;
            case PRIV_SOCK_PASV_ACCEPT:
                privop_pasv_accept(sess);
                break;
            default:
                fprintf(stderr, "Unkown command\n");
                exit(EXIT_FAILURE);
        }

    }
}

void set_nobody()
{
    struct passwd *pw;
    /*
    *    struct passwd {
    *　　char * pw_name;    Username.
    *　　char * pw_passwd;  Password.
    *　　uid_t  pw_uid;     User ID.
    *　　gid_t  pw_gid;     Group ID.
    *　　char * pw_gecos;   Real name.
    *　　char * pw_dir;     Home directory.
    *　　char * pw_shell;   Shell program.
　　*    };
    */
    /* 获取指定帐号为nobody在当前进程的相关信息 */
    if((pw = getpwnam("nobody")) == NULL)
        ERR_EXIT("getpwnam");

    /* 设置帐号nobody的gid为当前进程的有效组ID */
    if(setegid(pw->pw_gid) == -1)
        ERR_EXIT("setegid");

    /* 设置帐号nobody的uid为当前进程的有效用户ID */
    if(seteuid(pw->pw_uid) == -1)
        ERR_EXIT("seteuid");

}

void set_bind_capabilities()
{
    struct __user_cap_header_struct cap_user_header;
    cap_user_header.version = _LINUX_CAPABILITY_VERSION_1;
    cap_user_header.pid = getpid();

    struct __user_cap_data_struct cap_user_data;
    __u32 cap_mask = 0; //类似于权限的集合
    cap_mask |= (1 << CAP_NET_BIND_SERVICE); //0001000000
    cap_user_data.effective = cap_mask;
    cap_user_data.permitted = cap_mask;
    cap_user_data.inheritable = 0; //子进程不继承特权

    if(capset(&cap_user_header, &cap_user_data) == -1)
        ERR_EXIT("capset");
}

int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
    return syscall(SYS_capset, hdrp, datap);
}
