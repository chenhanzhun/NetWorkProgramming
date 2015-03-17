/********************
 * 目录操作：
 * 1、打开目录opendir
 *
 *  函数功能：打开一个目录；
 *  返回值：若成功返回一个目录指针，若出错则返回NULL指针；
 *  函数原型：
 *  #include <sys/types.h>
 *  #include <dirent.h>
 *  DIR *opendir(const char *pathname);
 *
 * 2、读一个目录readdir
 *
 *  函数功能：读取目录
 *  返回值：若成功返回指针，若在目录结尾或出错则返回NULL；
 *  函数原型：
 *  #include <unistd.h>
 *  #include <dirent.h>
 *  struct dirent *readdir(DIR *dp);
 *
 * 3、重设与dp关联的目录中的位置
 *
 *  函数功能：重设与dp关联的在目录中的位置；
 *  没有返回值；
 *  函数原型：
 *  #include <sys/types.h>
 *  #include <dirent.h>
 *  void rewinddir(DIR *dp);
 *
 * 4、dp关联的在目录的当前位置return current location in directory stream.
 *
 * 函数功能：获取与dp关联的在目录中的当前位置；
 *  返回值：若成功返回与dp关联的在目录中的当前位置，若出错返回-1；
 *  函数原型：
 *  #include <dirent.h>
 *  off_t telldir(DIR *dp);
 *
 * 5、The seekdir() function sets the location in the  directory
 *    stream  from  which  the  next  readdir() call will start.
 *    seekdir() should  be  used  with  an  offset  returned  by telldir().
 *
 *   函数功能：设置下一个readdir调用在目录的位置
 *    无返回值
 *    函数原型：
 *    #include <dirent.h>
 *    void seekdir(DIR *dp, off_t offset);
 *
 * 6、关闭一个已打开的目录
 *
 *  函数功能：关闭一个已打开的目录
 *  返回值：若成功返回0，若出错返回-1；
 *  函数原型：
 *  #include <sys/types.h>
 *  #include <dirent.h>
 *  int closedir(DIR *dp);
 *  **********************************/

/************************************
 * struct dirent结构信息
 * *******/
   struct dirent
    {
        long d_ino;                 /* inode number */
        off_t d_off;                /* offset to this dirent */
        unsigned short d_reclen;    /* length of this d_name */
        char d_name [NAME_MAX+1];   /* file name (null-terminated) */
    };
/***********
 * 目录创建
 * 函数功能：创建一个空目录
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * #include <sys/stat.h>
 * int mkdir(const char *pathname, mode_t mode);
 * 说明：
 * 此函数创建一个新的空目录，其中.和..目录项是自动创建的。
 * 所指定的文件访问权限mode由进程的文件模式创建屏蔽字修改。
 *
 * 删除空目录
 * 函数功能：删除一个空的目录
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * #include <unistd.h>
 * int rmdir(const char *pathname);
 * 说明：
 * 如果调用该函数使目录的链接数为0，且没有其他进程打开该目录，则释放该目录空间。
 * 若链接数为0，此时有进程打开该目录，在进程关闭前不会释放目录空间。
 * ***************/

/*************************
 * 更改当前工作目录。
 * 函数功能：更改当前的工作目录；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * #include <unistd.h>
 * int chdir(const char *pathname);
 * int fchdir(int filedes);
 * 说明：
 * 这两个函数分别用pathname或打开文件描述符来指定当前的工作目录。
 * **********/
/*************
 * 获取当前工作目录的绝对路径
 * 函数原型：
 * #include <unistd.h>
 * char *getcwd(char *buf, size_t size);
 * ***************/
