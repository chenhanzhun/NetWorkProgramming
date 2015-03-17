#include <stdio.h>
#include "ngx_queue.h"
#include "ngx_conf_file.h"
#include "ngx_config.h"
#include "ngx_palloc.h"
#include "nginx.h"
#include "ngx_core.h"

#define MAX     10
typedef struct Score
{
    unsigned int score;
    ngx_queue_t Que;
}ngx_queue_score;
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,  
            const char *fmt, ...)
{
}

ngx_int_t CMP(const ngx_queue_t *x, const ngx_queue_t *y)
{
    ngx_queue_score *xinfo = ngx_queue_data(x, ngx_queue_score, Que);
    ngx_queue_score *yinfo = ngx_queue_data(y, ngx_queue_score, Que);

    return(xinfo->score > yinfo->score);
}

void print_ngx_queue(ngx_queue_t *queue)
{
    ngx_queue_t *q = ngx_queue_head(queue);

    printf("score: ");
    for( ; q != ngx_queue_sentinel(queue); q = ngx_queue_next(q))
    {
        ngx_queue_score *ptr = ngx_queue_data(q, ngx_queue_score, Que);
        if(ptr != NULL)
            printf(" %d\t", ptr->score);
    }
    printf("\n");
}

int main()
{
    ngx_pool_t *pool;
    ngx_queue_t *queue;
    ngx_queue_score *Qscore;

    pool = ngx_create_pool(1024, NULL);

    queue = ngx_palloc(pool, sizeof(ngx_queue_t));
    ngx_queue_init(queue);

    int i;
    for(i = 1; i < MAX; i++)
    {
        Qscore = (ngx_queue_score*)ngx_palloc(pool, sizeof(ngx_queue_score));
        Qscore->score = i;
        ngx_queue_init(&Qscore->Que);

        if(i%2)
        {
            ngx_queue_insert_tail(queue, &Qscore->Que);
        }
        else
        {
            ngx_queue_insert_head(queue, &Qscore->Que);
        }
    }

    printf("Before sort: ");
    print_ngx_queue(queue);

    ngx_queue_sort(queue, CMP);

    printf("After sort: ");
    print_ngx_queue(queue);

    ngx_destroy_pool(pool);
    return 0;

}
