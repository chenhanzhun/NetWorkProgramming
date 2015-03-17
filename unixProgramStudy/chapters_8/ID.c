/* 进程ID */

/*
 * 函数功能：获取进程ID；
 * 返回值：所调用进程的ID；
 * 函数原型：
 */
  #include <unistd.h>

  pid_t getpid(void);

  /*
   * 函数功能：获取父进程ID；
   * 返回值：所调用进程的父进程ID；
   * 函数原型：
   */
  pid_t getppid(void);

  /*
   * 函数功能：进程实际用户ID；
   * 返回值：所调用进程的实际用户ID；
   * 函数原型：
   */
  uid_t getuid(void);

  /*
   * 函数功能：进程有效用户ID；
   * 返回值：所调用进程的有效用户ID；
   * 函数原型：
   */
  uid_t geteuid(void);

  /*
   * 函数功能：进程实际组ID；
   * 返回值：所调用进程的实际组ID；
   * 函数原型：
   */
  gid_t getgid(void);

  /*
   * 函数功能：进程有效组ID；
   * 返回值：所调用进程的有效组ID；
   * 函数原型：
   */
  gid_t getegid(void);
