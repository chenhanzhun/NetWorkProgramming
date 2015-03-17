/* 链表结构 */
typedef struct ngx_list_part_s  ngx_list_part_t;

/* 链表中的节点结构 */
struct ngx_list_part_s {
    void             *elts; /* 指向该节点数据区的首地址 */
    ngx_uint_t        nelts;/* 该节点数据区实际存放的元素个数 */
    ngx_list_part_t  *next; /* 指向链表的下一个节点 */
};

/* 链表表头结构 */
typedef struct {
    ngx_list_part_t  *last; /* 指向链表中最后一个节点 */
    ngx_list_part_t   part; /* 链表中表头包含的第一个节点 */
    size_t            size; /* 元素的字节大小 */
    ngx_uint_t        nalloc;/* 链表中每个节点所能容纳元素的个数 */
    ngx_pool_t       *pool; /* 该链表节点空间的内存池对象 */
} ngx_list_t;

/* 创建链表 */
ngx_list_t *
ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    ngx_list_t  *list;

    /* 分配链表表头的内存 */
    list = ngx_palloc(pool, sizeof(ngx_list_t));
    if (list == NULL) {
        return NULL;
    }

    /* 初始化链表 */
    if (ngx_list_init(list, pool, n, size) != NGX_OK) {
        return NULL;
    }

    return list;
}

/* 初始化链表 */
static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /* 分配节点数据区内存，并返回该节点数据区的首地址 */
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    /* 初始化节点成员 */
    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}

/* 添加一个元素 */
void *
ngx_list_push(ngx_list_t *l)
{
    void             *elt;
    ngx_list_part_t  *last;

    /* last节点指针指向链表最后一个节点 */
    last = l->last;

    /* 若最后一个节点的数据区已满 */
    if (last->nelts == l->nalloc) {

        /* the last part is full, allocate a new list part */

        /* 则分配一个新的节点 */
        last = ngx_palloc(l->pool, sizeof(ngx_list_part_t));
        if (last == NULL) {
            return NULL;
        }

        /* 分配新节点数据区内存，并使节点结构指向该数据区的首地址 */
        last->elts = ngx_palloc(l->pool, l->nalloc * l->size);
        if (last->elts == NULL) {
            return NULL;
        }

        /* 初始化新节点结构 */
        last->nelts = 0;
        last->next = NULL;

        /* 把新节点连接到现有链表中 */
        l->last->next = last;
        l->last = last;
    }

    /* 计算存储新元素的位置 */
    elt = (char *) last->elts + l->size * last->nelts;
    last->nelts++;  /* 实际存放元素加1 */

    /* 返回新元素所在位置 */
    return elt;
}
