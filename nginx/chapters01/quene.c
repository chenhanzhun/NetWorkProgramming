/* 队列结构，其实质是具有有头节点的双向循环链表 */
typedef struct ngx_queue_s  ngx_queue_t;

/* 队列中每个节点结构，只有两个指针，并没有数据区 */
struct ngx_queue_s {
    ngx_queue_t  *prev;
    ngx_queue_t  *next;
};

/* 初始化队列，即节点指针都指向自己，表示为空队列  */
#define ngx_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

/* 判断队列是否为空 */
#define ngx_queue_empty(h)                                                    \
    (h == (h)->prev)

/* 在队列头节点的下一节点插入新节点，其中h为头节点，x为新节点 */
#define ngx_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define ngx_queue_insert_after   ngx_queue_insert_head

/* 在队列尾节点之后插入新节点，其中h为尾节点，x为新节点 */
#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x

/* 获取队列头节点 */
#define ngx_queue_head(h)                                                     \
    (h)->next

/* 获取队列尾节点 */
#define ngx_queue_last(h)                                                     \
    (h)->prev


#define ngx_queue_sentinel(h)                                                 \
    (h)

/* 获取队列指定节点的下一个节点 */
#define ngx_queue_next(q)                                                     \
    (q)->next

/* 获取队列指定节点的前一个节点 */
#define ngx_queue_prev(q)                                                     \
    (q)->prev


#if (NGX_DEBUG)

#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next;                                              \
    (x)->prev = NULL;                                                         \
    (x)->next = NULL

#else
/* 删除队列指定的节点 */
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#endif

/* 拆分队列链表，使其称为两个独立的队列链表；
 * 其中h是原始队列的头节点，q是原始队列中的一个元素节点，n是新的节点，
 * 拆分后，原始队列以q为分界，头节点h到q之前的节点作为一个队列（不包括q节点），
 * 另一个队列是以n为头节点，以节点q及其之后的节点作为新的队列链表；
 */
#define ngx_queue_split(h, q, n)                                              \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

/* 合并两个队列链表，把n队列链表连接到h队列链表的尾部 */
#define ngx_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (n)->prev = (n)->next = n;/* 这是我个人增加的语句，若加上该语句，就不会出现头节点n会指向队列链表的节点 */

/* 返回q在所属结构类型的地址，type是链表元素的结构类型 */
#define ngx_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))
/*
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */

/* 返回队列链表中心元素 */
ngx_queue_t *
ngx_queue_middle(ngx_queue_t *queue)
{
    ngx_queue_t  *middle, *next;

    /* 获取队列链表头节点 */
    middle = ngx_queue_head(queue);

    /* 若队列链表的头节点就是尾节点，表示该队列链表只有一个元素 */
    if (middle == ngx_queue_last(queue)) {
        return middle;
    }

    /* next作为临时指针，首先指向队列链表的头节点 */
    next = ngx_queue_head(queue);

    for ( ;; ) {
        /* 若队列链表不止一个元素，则等价于middle = middle->next */
        middle = ngx_queue_next(middle);

        next = ngx_queue_next(next);

        /* 队列链表有偶数个元素 */
        if (next == ngx_queue_last(queue)) {
            return middle;
        }

        next = ngx_queue_next(next);

        /* 队列链表有奇数个元素 */
        if (next == ngx_queue_last(queue)) {
            return middle;
        }
    }
}


/* the stable insertion sort */

/* 队列链表排序 */
void
ngx_queue_sort(ngx_queue_t *queue,
    ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *))
{
    ngx_queue_t  *q, *prev, *next;

    q = ngx_queue_head(queue);

    /* 若队列链表只有一个元素，则直接返回 */
    if (q == ngx_queue_last(queue)) {
        return;
    }

    /* 遍历整个队列链表 */
    for (q = ngx_queue_next(q); q != ngx_queue_sentinel(queue); q = next) {

        prev = ngx_queue_prev(q);
        next = ngx_queue_next(q);

        /* 首先把元素节点q独立出来 */
        ngx_queue_remove(q);

        /* 找到适合q插入的位置 */
        do {
            if (cmp(prev, q) <= 0) {
                break;
            }

            prev = ngx_queue_prev(prev);

        } while (prev != ngx_queue_sentinel(queue));

        /* 插入元素节点q */
        ngx_queue_insert_after(prev, q);
    }
}

/* h 为链表结构体 ngx_queue_t 的指针；初始化双链表 */
ngx_queue_int(h)

/* h 为链表容器结构体 ngx_queue_t 的指针； 判断链表是否为空 */
ngx_queue_empty(h)

/* h 为链表容器结构体 ngx_queue_t 的指针，x 为插入元素结构体中 ngx_queue_t 成员的指针；将 x 插入到链表头部 */
ngx_queue_insert_head(h, x)

/* h 为链表容器结构体 ngx_queue_t 的指针，x 为插入元素结构体中 ngx_queue_t 成员的指针。将 x 插入到链表尾部 */
ngx_queue_insert_tail(h, x)

/* h 为链表容器结构体 ngx_queue_t 的指针。返回链表容器 h 中的第一个元素的 ngx_queue_t 结构体指针 */
ngx_queue_head(h)

/* h 为链表容器结构体 ngx_queue_t 的指针。返回链表容器 h 中的最后一个元素的 ngx_queue_t 结构体指针 */
ngx_queue_last(h)

/* h 为链表容器结构体 ngx_queue_t 的指针。返回链表结构体的指针 */
ngx_queue_sentinel(h)

/* x 为链表容器结构体 ngx_queue_t 的指针。从容器中移除 x 元素 */
ngx_queue_remove(x)

/* h 为链表容器结构体 ngx_queue_t 的指针。该函数用于拆分链表，
 * h 是链表容器，而 q 是链表 h 中的一个元素。
 * 将链表 h 以元素 q 为界拆分成两个链表 h 和 n
 */
ngx_queue_split(h, q, n)

/* h 为链表容器结构体 ngx_queue_t 的指针， n为另一个链表容器结构体 ngx_queue_t 的指针
 * 合并链表，将 n 链表添加到 h 链表的末尾
 */
ngx_queue_add(h, n)

/* h 为链表容器结构体 ngx_queue_t 的指针。返回链表中心元素，即第 N/2 + 1 个 */
ngx_queue_middle(h)

/* h 为链表容器结构体 ngx_queue_t 的指针，cmpfunc 是比较回调函数。使用插入排序对链表进行排序 */
ngx_queue_sort(h, cmpfunc)

/* q 为链表中某一个元素结构体的 ngx_queue_t 成员的指针。返回 q 元素的下一个元素。*/
ngx_queue_next(q)

/* q 为链表中某一个元素结构体的 ngx_queue_t 成员的指针。返回 q 元素的上一个元素。*/
ngx_queue_prev(q)

/* q 为链表中某一个元素结构体的 ngx_queue_t 成员的指针，type 是链表元素的结构体类型名称，
 * link 是上面这个结构体中 ngx_queue_t 类型的成员名字。返回 q 元素所属结构体的地址
 */
ngx_queue_data(q, type, link)

/* q 为链表中某一个元素结构体的 ngx_queue_t 成员的指针，x 为插入元素结构体中 ngx_queue_t 成员的指针 */
ngx_queue_insert_after(q, x)
