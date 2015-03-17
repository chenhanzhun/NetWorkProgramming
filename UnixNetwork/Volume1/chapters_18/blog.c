struct sockaddr_dl {
  uint8_t      sdl_len;
  sa_family_t  sdl_family;   /* AF_LINK */
  uint16_t     sdl_index;    /* system assigned index, if > 0 */
  uint8_t      sdl_type;     /* IFT_ETHER, etc. from <net/if_types.h> */
  uint8_t      sdl_nlen;     /* name length, starting in sdl_data[0] */
  uint8_t      sdl_alen;     /* link-layer address length */
  uint8_t      sdl_slen;     /* link-layer selector length */
  char         sdl_data[12]; /* minimum work area, can be larger;
                                contains i/f name and link-layer address */
};
/* 说明：
 * 其中 sdl_data成员包含名字和链路层地址；名字从sdl_data[0]开始，而且不以空字符结尾；
 * 链路层地址从sdl_data[sdl_nlen]开始；下面宏定义是返回指向链路层地址的指针：
 */
#define LLADDR(s)   ((caddr_t)((s)->sdl_data + (s)->sdl_nlen))


/* 函数功能：检查路由表和接口列表；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/param.h>
#include <sys/sysctl.h>
int sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen);
/* 说明：
 * 参数name是指定名字的一个整数数组，namelen是指定该数组中的元素数目，该数组中第一个元素指定请求定向到内核的某个子系统，接下来的元素逐渐细化指定该子系统的某个部分；
 * oldp指向提供内核存放值的缓冲区，oldlenp是值-结果参数，即指定缓冲区大小作为参数值输入，函数返回时，该值结果变为内核存放在该缓冲区的实际大小；
 * newp指向一个为newlen参数值的缓冲区，也可指定为空指针；
 *
 */


/*name 参数的第一个元素可取以下值：*/
/* Top-level names: */

enum
{
	CTL_KERN=1,		/* General kernel info and control */
	CTL_VM=2,		/* VM management */
	CTL_NET=3,		/* Networking */
	CTL_PROC=4,		/* removal breaks strace(1) compilation */
	CTL_FS=5,		/* Filesystems */
	CTL_DEBUG=6,		/* Debugging */
	CTL_DEV=7,		/* Devices */
	CTL_BUS=8,		/* Busses */
	CTL_ABI=9,		/* Binary emulation */
	CTL_CPU=10,		/* CPU stuff (speed scaling, etc) */
	CTL_ARLAN=254,		/* arlan wireless driver */
	CTL_S390DBF=5677,	/* s390 debug */
	CTL_SUNRPC=7249,	/* sunrpc debug */
	CTL_PM=9899,		/* frv power management */
	CTL_FRV=9898,		/* frv specific sysctls */
};
/* name 参数的第二个元素可取以下值：
 * （1）AF_INET     获取或设置影响网际网协议的变量；
 * （2）AF_LINK     获取或设置链路层信息；
 * （3）AF_ROUTE    返回路由表或接口列表信息；
 * （4）AF_UNSPEC   获取或设置一些套接字层变量；
 */
 /* 若name数组的第二个元素指定为AF_ROUTE时，第三个元素则是0，第四个元素则是一个地址族，第五个和第六个元素取值如下表； */


/* 接口名字和索引函数 */

#include <net/if.h>
/* Convert an interface name to an index, and vice versa.  */
unsigned int if_nametoindex (const char *ifname);/* 若成功则返回正的接口索引，出错返回0 */
char *if_indextoname (unsigned int ifindex, char *ifname);/* 若成功则返回指向接口名字的指针，出错则返回NULL */

/* Return a list of all interfaces and their indices.  */
struct if_nameindex *if_nameindex (void);/* 若成功则返回非空指针，出错则返回NULL */

/* Free the data returned from if_nameindex.  */
void if_freenameindex (struct if_nameindex *ptr);
/* 说明：
 * if_nametoindex返回名为ifname的接口的索引；
 * if_indextoname对给定的ifindex返回一个指向其接口名的指针，ifname参数指向一个大小为IFNAMSIZ头文件中定义的缓冲区，
 * 调用者必须分配这个缓冲区以保存结果，成功时这个指针也是函数的返回值；
 * if_nameindex返回一个指向if_nameindex结构的数组的指针；
 */
/* 结构体if_nameindex 的定义 */
struct if_nameindex
  {
    unsigned int if_index;	/* 1, 2, ... */
    char *if_name;		/* null terminated name: "eth0", ... */
  };/* 结构成员内存空间由函数动态获取，并由函数if_freenameindex归还给系统 */
