
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CONF_FILE_H_INCLUDED_
#define _NGX_CONF_FILE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 *        AAAA  number of arguments
 *      FF      command flags
 *    TT        command type, i.e. HTTP "location" or "server" command
 */

#define NGX_CONF_NOARGS      0x00000001
#define NGX_CONF_TAKE1       0x00000002
#define NGX_CONF_TAKE2       0x00000004
#define NGX_CONF_TAKE3       0x00000008
#define NGX_CONF_TAKE4       0x00000010
#define NGX_CONF_TAKE5       0x00000020
#define NGX_CONF_TAKE6       0x00000040
#define NGX_CONF_TAKE7       0x00000080

#define NGX_CONF_MAX_ARGS    8

#define NGX_CONF_TAKE12      (NGX_CONF_TAKE1|NGX_CONF_TAKE2)
#define NGX_CONF_TAKE13      (NGX_CONF_TAKE1|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE23      (NGX_CONF_TAKE2|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE123     (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3)
#define NGX_CONF_TAKE1234    (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3   \
                              |NGX_CONF_TAKE4)

#define NGX_CONF_ARGS_NUMBER 0x000000ff
#define NGX_CONF_BLOCK       0x00000100
#define NGX_CONF_FLAG        0x00000200
#define NGX_CONF_ANY         0x00000400
#define NGX_CONF_1MORE       0x00000800
#define NGX_CONF_2MORE       0x00001000
#define NGX_CONF_MULTI       0x00000000  /* compatibility */

#define NGX_DIRECT_CONF      0x00010000

#define NGX_MAIN_CONF        0x01000000
#define NGX_ANY_CONF         0x0F000000



#define NGX_CONF_UNSET       -1
#define NGX_CONF_UNSET_UINT  (ngx_uint_t) -1
#define NGX_CONF_UNSET_PTR   (void *) -1
#define NGX_CONF_UNSET_SIZE  (size_t) -1
#define NGX_CONF_UNSET_MSEC  (ngx_msec_t) -1


#define NGX_CONF_OK          NULL
#define NGX_CONF_ERROR       (void *) -1

#define NGX_CONF_BLOCK_START 1
#define NGX_CONF_BLOCK_DONE  2
#define NGX_CONF_FILE_DONE   3

#define NGX_CORE_MODULE      0x45524F43  /* "CORE" */
#define NGX_CONF_MODULE      0x464E4F43  /* "CONF" */


#define NGX_MAX_CONF_ERRSTR  1024


struct ngx_command_s {
    /* 配置项名称 */
    ngx_str_t             name;
    /* 配置项类型，type将指定配置项可以出现的位置以及携带参数的个数 */
    ngx_uint_t            type;
    /* 处理配置项的参数 */
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    /* 在配置文件中的偏移量，conf与offset配合使用 */
    ngx_uint_t            conf;
    ngx_uint_t            offset;
    /* 配置项读取后的处理方法，必须指向ngx_conf_post_t 结构 */
    void                 *post;
};

#define ngx_null_command  { ngx_null_string, 0, NULL, 0, 0, NULL }


struct ngx_open_file_s {
    ngx_fd_t              fd;
    ngx_str_t             name;

    void                (*flush)(ngx_open_file_t *file, ngx_log_t *log);
    void                 *data;
};


/* Nginx 模块的数据结构 */
#define NGX_MODULE_V1          0, 0, 0, 0, 0, 0, 1
#define NGX_MODULE_V1_PADDING  0, 0, 0, 0, 0, 0, 0, 0

struct ngx_module_s {
    /* 模块类别由type成员决定，ctx_index表示当前模块在type类模块中的序号 */
    ngx_uint_t            ctx_index;
    /* index 区别与ctx_index，index表示当前模块在所有模块中的序号 */
    ngx_uint_t            index;

    /* spare 序列保留变量，暂时不被使用 */
    ngx_uint_t            spare0;
    ngx_uint_t            spare1;
    ngx_uint_t            spare2;
    ngx_uint_t            spare3;

    /* 当前模块的版本 */
    ngx_uint_t            version;

    /* ctx指向特定类型模块的公共接口，例如在HTTP模块中，ctx指向ngx_http_module_t结构体 */
    void                 *ctx;
    /* 处理nginx.conf中的配置项 */
    ngx_command_t        *commands;
    /* type表示当前模块的类型 */
    ngx_uint_t            type;

    /* 下面的7个函数指针是在Nginx启动或停止时，分别调用的7中方法 */
    /* 在master进程中回调init_master */
    ngx_int_t           (*init_master)(ngx_log_t *log);

    /* 初始化所有模块时回调init_module */
    ngx_int_t           (*init_module)(ngx_cycle_t *cycle);

    /* 在worker进程提供正常服务之前回调init_process初始化进程 */
    ngx_int_t           (*init_process)(ngx_cycle_t *cycle);
    /* 初始化多线程 */
    ngx_int_t           (*init_thread)(ngx_cycle_t *cycle);
    /* 退出多线程 */
    void                (*exit_thread)(ngx_cycle_t *cycle);
    /* 在worker进程停止服务之前回调exit_process */
    void                (*exit_process)(ngx_cycle_t *cycle);

    /* 在master进程退出之前回调exit_master */
    void                (*exit_master)(ngx_cycle_t *cycle);

    /* 保留字段，未被使用 */
    uintptr_t             spare_hook0;
    uintptr_t             spare_hook1;
    uintptr_t             spare_hook2;
    uintptr_t             spare_hook3;
    uintptr_t             spare_hook4;
    uintptr_t             spare_hook5;
    uintptr_t             spare_hook6;
    uintptr_t             spare_hook7;
};


typedef struct{
    /* 解析配置文件之前被调用 */
    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
    /* 完成配置文件的解析后被调用 */
    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);

    /* 创建存储main级别的全局配置项的结构体（直属于http块） */
    void        *(*create_main_conf)(ngx_conf_t *cf);
    /* 初始化main级别的配置项 */
    char        *(*init_main_conf)(ngx_conf_t *cf);

    /* 创建存储srv级别的配置项的结构体（直属于server块） */
    void        *(*create_srv_conf)(ngx_conf_t *cf);
    /* 合并main级别与srv级别下的同名配置项 */
    char        *(*merge_srv_conf)(ngx_conf_t *cf);

    /* 创建存储loc级别的配置项的结构体（直属于location块） */
    void        *(*create_loc_conf)(ngx_conf_t *cf);
    /* 合并srv级别与loc级别下的同名配置项 */
    char        *(*merge_loc_conf)(ngx_conf_t *cf);
}ngx_http_module_t;

/* 核心模块的通用接口结构体 */
typedef struct {
    /* 模块名称 */
    ngx_str_t             name;
    /* 解析配置项前，调用该方法 */
    void               *(*create_conf)(ngx_cycle_t *cycle);
    /* 完成配置项解析后，调用该函数 */
    char               *(*init_conf)(ngx_cycle_t *cycle, void *conf);
} ngx_core_module_t;


typedef struct {
    ngx_file_t            file;     /* 文件的属性 */
    ngx_buf_t            *buffer;   /* 文件的内容 */
    ngx_uint_t            line;     /* 文件的行数 */
} ngx_conf_file_t;


typedef char *(*ngx_conf_handler_pt)(ngx_conf_t *cf,
    ngx_command_t *dummy, void *conf);


/* 解析配置时所使用的结构体 */
struct ngx_conf_s {
    char                 *name;     /* 当前解析到的指令 */
    ngx_array_t          *args;     /* 当前指令所包含的所有参数 */

    ngx_cycle_t          *cycle;    /* 待解析的全局变量ngx_cycle_t */
    ngx_pool_t           *pool;     /* 内存池 */
    ngx_pool_t           *temp_pool;/* 临时内存池，分配一些临时数组或变量 */
    ngx_conf_file_t      *conf_file;/* 待解析的配置文件 */
    ngx_log_t            *log;      /* 日志信息 */

    void                 *ctx;      /* 描述指令的上下文 */
    ngx_uint_t            module_type;/* 当前解析的指令的模块类型 */
    ngx_uint_t            cmd_type; /* 当前解析的指令的指令类型 */

    ngx_conf_handler_pt   handler;  /* 模块自定义的handler，即指令自定义的处理函数 */
    char                 *handler_conf;/* 自定义处理函数需要的相关配置 */
};


typedef char *(*ngx_conf_post_handler_pt) (ngx_conf_t *cf,
    void *data, void *conf);

typedef struct {
    ngx_conf_post_handler_pt  post_handler;
} ngx_conf_post_t;


typedef struct {
    ngx_conf_post_handler_pt  post_handler;
    char                     *old_name;
    char                     *new_name;
} ngx_conf_deprecated_t;


typedef struct {
    ngx_conf_post_handler_pt  post_handler;
    ngx_int_t                 low;
    ngx_int_t                 high;
} ngx_conf_num_bounds_t;


typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                value;
} ngx_conf_enum_t;


#define NGX_CONF_BITMASK_SET  1

typedef struct {
    ngx_str_t                 name;
    ngx_uint_t                mask;
} ngx_conf_bitmask_t;



char * ngx_conf_deprecated(ngx_conf_t *cf, void *post, void *data);
char *ngx_conf_check_num_bounds(ngx_conf_t *cf, void *post, void *data);


#define ngx_get_conf(conf_ctx, module)  conf_ctx[module.index]



#define ngx_conf_init_value(conf, default)                                   \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = default;                                                      \
    }

#define ngx_conf_init_ptr_value(conf, default)                               \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = default;                                                      \
    }

#define ngx_conf_init_uint_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_size_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_init_msec_value(conf, default)                              \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = default;                                                      \
    }

#define ngx_conf_merge_value(conf, prev, default)                            \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_ptr_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET_PTR) {                                        \
        conf = (prev == NGX_CONF_UNSET_PTR) ? default : prev;                \
    }

#define ngx_conf_merge_uint_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_UINT) {                                       \
        conf = (prev == NGX_CONF_UNSET_UINT) ? default : prev;               \
    }

#define ngx_conf_merge_msec_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_MSEC) {                                       \
        conf = (prev == NGX_CONF_UNSET_MSEC) ? default : prev;               \
    }

#define ngx_conf_merge_sec_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_size_value(conf, prev, default)                       \
    if (conf == NGX_CONF_UNSET_SIZE) {                                       \
        conf = (prev == NGX_CONF_UNSET_SIZE) ? default : prev;               \
    }

#define ngx_conf_merge_off_value(conf, prev, default)                        \
    if (conf == NGX_CONF_UNSET) {                                            \
        conf = (prev == NGX_CONF_UNSET) ? default : prev;                    \
    }

#define ngx_conf_merge_str_value(conf, prev, default)                        \
    if (conf.data == NULL) {                                                 \
        if (prev.data) {                                                     \
            conf.len = prev.len;                                             \
            conf.data = prev.data;                                           \
        } else {                                                             \
            conf.len = sizeof(default) - 1;                                  \
            conf.data = (u_char *) default;                                  \
        }                                                                    \
    }

#define ngx_conf_merge_bufs_value(conf, prev, default_num, default_size)     \
    if (conf.num == 0) {                                                     \
        if (prev.num) {                                                      \
            conf.num = prev.num;                                             \
            conf.size = prev.size;                                           \
        } else {                                                             \
            conf.num = default_num;                                          \
            conf.size = default_size;                                        \
        }                                                                    \
    }

#define ngx_conf_merge_bitmask_value(conf, prev, default)                    \
    if (conf == 0) {                                                         \
        conf = (prev == 0) ? default : prev;                                 \
    }


char *ngx_conf_param(ngx_conf_t *cf);
char *ngx_conf_parse(ngx_conf_t *cf, ngx_str_t *filename);
char *ngx_conf_include(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


ngx_int_t ngx_conf_full_name(ngx_cycle_t *cycle, ngx_str_t *name,
    ngx_uint_t conf_prefix);
ngx_open_file_t *ngx_conf_open_file(ngx_cycle_t *cycle, ngx_str_t *name);
void ngx_cdecl ngx_conf_log_error(ngx_uint_t level, ngx_conf_t *cf,
    ngx_err_t err, const char *fmt, ...);


char *ngx_conf_set_flag_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_array_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_conf_set_keyval_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_num_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_size_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_off_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_msec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_sec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bufs_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_enum_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bitmask_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


extern ngx_uint_t     ngx_max_module;
extern ngx_module_t  *ngx_modules[];


#endif /* _NGX_CONF_FILE_H_INCLUDED_ */
