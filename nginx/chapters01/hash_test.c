#include <stdio.h>
#include "ngx_config.h"
#include "ngx_conf_file.h"
#include "nginx.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"
#include "ngx_array.h"
#include "ngx_hash.h"
volatile ngx_cycle_t  *ngx_cycle;
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err, const char *fmt, ...) { }


static ngx_str_t names[] = {ngx_string("www.baidu.com"),
                            ngx_string("www.google.com.hk"),
                            ngx_string("www.github.com")};
static char* descs[] = {"baidu: 1","google: 2", "github: 3"};

// hash table的一些基本操作
int main()
{
    ngx_uint_t          k;
    ngx_pool_t*         pool;
    ngx_hash_init_t     hash_init;
    ngx_hash_t*         hash;
    ngx_array_t*        elements;
    ngx_hash_key_t*     arr_node;
    char*               find;
    int                 i;
    ngx_cacheline_size = 32;

    pool = ngx_create_pool(1024*10, NULL);

    /* 分配hash表基本结构内存 */
    hash = (ngx_hash_t*) ngx_pcalloc(pool, sizeof(hash));
    /* 初始化hash结构 */
    hash_init.hash      = hash;                      // hash结构
    hash_init.key       = &ngx_hash_key_lc;          // hash函数
    hash_init.max_size  = 1024*10;                   // max_size
    hash_init.bucket_size = 64;
    hash_init.name      = "test_hash_error";
    hash_init.pool           = pool;
    hash_init.temp_pool      = NULL;

    /* 创建数组，把关键字压入到数组中 */

    elements = ngx_array_create(pool, 32, sizeof(ngx_hash_key_t));
    for(i = 0; i < 3; i++) {
        arr_node            = (ngx_hash_key_t*) ngx_array_push(elements);
        arr_node->key       = (names[i]);
        arr_node->key_hash  = ngx_hash_key_lc(arr_node->key.data, arr_node->key.len);
        arr_node->value     = (void*) descs[i];

        printf("key: %s , key_hash: %u\n", arr_node->key.data, arr_node->key_hash);
    }

    /* hash初始化函数 */
    if (ngx_hash_init(&hash_init, (ngx_hash_key_t*) elements->elts, elements->nelts) != NGX_OK){
        return 1;
    }

    /* 查找hash 元素 */
    k    = ngx_hash_key_lc(names[0].data, names[0].len);
    printf("%s key is %d\n", names[0].data, k);
    find = (char*)
        ngx_hash_find(hash, k, (u_char*) names[0].data, names[0].len);

    if (find) {
        printf("get desc: %s\n",(char*) find);
    }

    ngx_array_destroy(elements);
    ngx_destroy_pool(pool);

    return 0;
}
