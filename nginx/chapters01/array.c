/* 基本数据结构 */

/* Nginx 简单数据类型 */
/* 在文件 src/core/ngx_config.h 定义了基本的数据映射 */

typedef intptr_t        ngx_int_t;
typedef uintptr_t       ngx_uint_t;
typedef intptr_t        ngx_flag_t;
/* 其中 intptr_t uintptr_t 定义在文件 /usr/include/stdint.h 文件中*/

/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
# ifndef __intptr_t_defined
typedef long int		intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned long int	uintptr_t;
#else
# ifndef __intptr_t_defined
typedef int			intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned int		uintptr_t;

/* 因此，Nginx 的简单数据类型的操作和整型或指针类型类似 */

/* Nginx 字符串数据类型 */
/* Nginx 字符串类型是对 C 语言字符串类型的简单封装，
 * 其定义在 core/ngx_string.h 或 core/ngx_string.c 中
 * 定义了 ngx_str_t, ngx_keyval_t, ngx_variable_value_t
 */

/* ngx_str_t 在 u_char 的基础上增加了字符串长度的信息，即len变量 */
typedef struct {
    size_t      len;    /* 字符串的长度 */
    u_char     *data;   /* 指向字符串的第一个字符 */
} ngx_str_t;


typedef struct {
    ngx_str_t   key;
    ngx_str_t   value;
} ngx_keyval_t;


typedef struct {
    unsigned    len:28;

    unsigned    valid:1;
    unsigned    no_cacheable:1;
    unsigned    not_found:1;
    unsigned    escape:1;

    u_char     *data;
} ngx_variable_value_t;
/* Nginx 字符串的初始化使用 ngx_string 或 ngx_null_string ，这两个宏定义如下 */

#define ngx_string(str) {sizeof(str)-1, (u_char *) str}
#define ngx_null_string {0, NULL}

/* 若已经定义了 Nginx 字符串变量之后再赋值，则必须使用 ngx_str_set, ngx_str_null 宏定义*/

#define ngx_str_set(str, text)
    (str)->len = sizeof(text)-1; (str)->data = (u_char *)text

#define ngx_str_null(str)   (str)->len = 0; (str)->data = NULL

/* 例如：*/
/* 正确写法*/
ngx_str_t str1 = ngx_string("hello nginx");
ngx_str_t str2 = ngx_null_string;

/* 错误写法*/
ngx_str_t str1, str2;
str1 = ngx_string("hello nginx");   /* 编译出错 */
str2 = ngx_null_string;             /* 编译出错 */

/* 正确写法*/
ngx_str_t str1, str2;
ngx_str_set(&str1, "hello nginx");
ngx_str_null(&str2);
/* 注意：ngx_string 和 ngx_str_set 字符串参数必须是常量字符串，不能是变量字符串 */


/* 内存池结构 */
/* 文件 core/ngx_palloc.h */
typedef struct {/* 内存池数据结构模块 */
    u_char               *last; /* 当前内存分配的结束位置，即下一段可分配内存的起始位置 */
    u_char               *end;  /* 内存池的结束位置 */
    ngx_pool_t           *next; /* 指向下一个内存池 */
    ngx_uint_t            failed;/* 记录内存池内存分配失败的次数 */
} ngx_pool_data_t;  /* 维护内存池的数据块 */


struct ngx_pool_s {/* 内存池的管理模块，即内存池头部结构 */
    ngx_pool_data_t       d;    /* 内存池的数据块 */
    size_t                max;  /* 内存池数据块的最大值 */
    ngx_pool_t           *current;/* 指向当前内存池 */
    ngx_chain_t          *chain;/* 指向一个 ngx_chain_t 结构 */
    ngx_pool_large_t     *large;/* 大块内存链表，即分配空间超过 max 的内存 */
    ngx_pool_cleanup_t   *cleanup;/* 析构函数，释放内存池 */
    ngx_log_t            *log;/* 内存分配相关的日志信息 */
};
/* 文件 core/ngx_core.h */
typedef struct ngx_pool_s   ngx_pool_t;
typedef struct ngx_chain_s  ngx_chain_t;


typedef struct {
    void        *elts;  /* 指向数组数据区域的首地址 */
    ngx_uint_t   nelts; /* 数组实际数据的个数 */
    size_t       size;  /* 单个元素所占据的字节大小 */
    ngx_uint_t   nalloc;/* 数组容量 */
    ngx_pool_t  *pool;  /* 数组对象所在的内存池 */
} ngx_array_t;

/* 创建新的动态数组 */
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);
/* 销毁数组对象，内存被内存池回收 */
void ngx_array_destroy(ngx_array_t *a);
/* 在现有数组中增加一个新的元素 */
void *ngx_array_push(ngx_array_t *a);
/* 在现有数组中增加 n 个新的元素 */
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);


/* 创建动态数组对象 */
ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

    /* 分配动态数组指针内存 */
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

    /* 分配容量为 n 的动态数组*/
    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }

    return a;
}

/* 当一个数组对象被分配在堆上，且调用ngx_array_destroy之后，若想重新使用，则需调用该函数 */
/* 若数组对象被分配在栈上，则需调用此函数 */
static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */

    /* 初始化数组成员，注意：nelts必须比elts先初始化 */
    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    /* 分配数组数据域所需要的内存 */
    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}

/* 销毁数组对象，即数组所占据的内存被内存池回收 */
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

    /* 移动内存池的last，释放数组所有元素所占据的内存 */
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    /* 释放数组首指针所占据的内存 */
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}

/* 数组增加一个元素 */
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    /* 判断数组是否已满 */
    if (a->nelts == a->nalloc) {

        /* 若现有数组所容纳的元素个数已满 */
        /* the array is full */

        /* 计算数组所有元素占据的内存大小 */
        size = a->size * a->nalloc;

        p = a->pool;

        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
            /* 若当前内存池的内存空间至少可容纳一个元素大小 */
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* 若当前内存池不足以容纳一个元素，则分配新的数组内存 */
            /* allocate a new array */

            /* 新的数组内存为当前数组大小的 2 倍 */
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            /* 首先把现有数组的所有元素复制到新的数组中 */
            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    /* 返回指向新增加元素的指针 */
    return elt;
}


/* 数组增加 n 个元素 */
void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}

/* 缓冲区结构 */
typedef void *            ngx_buf_tag_t;

typedef struct ngx_buf_s  ngx_buf_t;

struct ngx_buf_s {
    u_char          *pos;   /* 缓冲区数据在内存的起始位置 */
    u_char          *last;  /* 缓冲区数据在内存的结束位置 */
    /* 这两个参数是处理文件时使用，类似于缓冲区的pos, last */
    off_t            file_pos;
    off_t            file_last;

    /* 由于实际数据可能被包含在多个缓冲区中，则缓冲区的start和end指向
     * 这块内存的开始地址和结束地址，
     * 而pos和last是指向本缓冲区实际包含的数据的开始和结尾
     */
    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */
    ngx_buf_tag_t    tag;
    ngx_file_t      *file;          /* 指向buffer对应的文件对象 */
    /* 当前缓冲区的一个影子缓冲区，即当一个缓冲区复制另一个缓冲区的数据，
     * 就会发生相互指向对方的shadow指针
     */
    ngx_buf_t       *shadow;


    /* 为1时，表示该buf所包含的内容在用户创建的内存块中
     * 可以被filter处理变更
     */
    /* the buf's content could be changed */
    unsigned         temporary:1;

    /* 为1时，表示该buf所包含的内容在内存中，不能被filter处理变更 */
    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;

    /* 为1时，表示该buf所包含的内容在内存中，
     * 可通过mmap把文件映射到内存中，不能被filter处理变更 */
    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;

    /* 可回收，即这些buf可被释放 */
    unsigned         recycled:1;
    unsigned         in_file:1; /* 表示buf所包含的内容在文件中 */
    unsigned         flush:1;   /* 刷新缓冲区 */
    unsigned         sync:1;    /* 同步方式 */
    unsigned         last_buf:1;/* 当前待处理的是最后一块缓冲区 */
    unsigned         last_in_chain:1;/* 在当前的chain里面，该buf是最后一个，但不一定是last_buf */

    unsigned         last_shadow:1;
    unsigned         temp_file:1;

    /* STUB */ int   num;
};


struct ngx_chain_s {
    ngx_buf_t    *buf;  /* 指向当前缓冲区 */
    ngx_chain_t  *next; /* 指向下一个chain，形成chain链表 */
};
typedef struct {
    ngx_int_t    num;
    size_t       size;
} ngx_bufs_t;

