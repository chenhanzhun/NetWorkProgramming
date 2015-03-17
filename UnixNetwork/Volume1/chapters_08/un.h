#ifndef UN_H
#define UN_H
#include <netinet/in.h>

/* 自定义结构体，成员包含目的地址、接口索引 */
struct unp_in_pktinfo{
    struct in_addr  ipi_addr;   /* destination IPv4 address */
    int             ipi_ifindex;/* received interface index */
};
#endif
