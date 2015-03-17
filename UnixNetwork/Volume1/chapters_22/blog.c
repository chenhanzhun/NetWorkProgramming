 #include <sys/socket.h>

    /* 宏定义 */
   struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh);
   struct cmsghdr *CMSG_NXTHDR(struct msghdr *msgh, struct cmsghdr *cmsg);
   size_t CMSG_ALIGN(size_t length);
   size_t CMSG_SPACE(size_t length);
   size_t CMSG_LEN(size_t length);
   unsigned char *CMSG_DATA(struct cmsghdr *cmsg);

   /* 结构 cmsghdr*/
   struct cmsghdr {
       socklen_t cmsg_len;    /* data byte count, including header */
       int       cmsg_level;  /* originating protocol */
       int       cmsg_type;   /* protocol-specific type */
       /* followed by unsigned char cmsg_data[]; */
   };
/* 这些宏定义用来创建和访问不是套接字负载部分的control控制信息(也称为辅助数据)；
 * 这些控制信息可能包含所接收数据报的接口和各种很少使用的描述头等信息；
 */
 CMSG_FIRSTHDR(); /* 返回指向辅助数据缓冲区的第一个cmsghdr结构的指针，若无辅助数据则返回NULL； */

 CMSG_NXTHDR();/* 返回指向下一个cmsghdr结构的指针，若不再有辅助对象则返回NULL；*/

 CMSG_ALIGN();/* given a length, returns it including the required  align‐
       ment.  This is a constant expression. */

 CMSG_SPACE();/* 返回给定数据量的一个辅助数据对象的大小 */

 CMSG_DATA();/* 返回指向与cmsghdr结构关联的数据的第一个字节的指针； */

 CMSG_LEN();/* 返回给定数据量的存放到cmsg_len中的值；*/
