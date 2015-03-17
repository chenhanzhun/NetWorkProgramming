/*************
 * 硬链接指令ln使用方式如下：
 * ln existingfile newfile
 * 说明：
 * 为当前存在的文件existingfile创建新的硬链接，链接文件名为newfile.
 * 举例子说明：
 * 列出当前文件存在的信息，包括关键inode节点信息：
 * $ ls -il file1 file2
 * 10890904 -rw-rw-r-- 1  file1
 * 10890903 -rw-rw-r-- 1  file2
 * $ ln file1 hardlink
 * $ ls -il file1 file2 hardlink
 * 10890904 -rw-rw-r-- 2  file1
 * 10890903 -rw-rw-r-- 1  file2
 * 10890904 -rw-rw-r-- 2  hardlink
 * $ cat file1 hardlink
 * Hello, this is the file1.
 * Hello, this is the file1.
 * $ rm file1
 * $ ls -il file2 hardlink
 * 10890903 -rw-rw-r-- 1  file2
 * 10890904 -rw-rw-r-- 1  hardlink
 * $ cat hardlink
 * Hello, this is the file1.
 *
 */
/*************
 * 符号链接指令ln使用方式如下：
 * ln -s existingfile newfile
 * 说明：
 * 为当前存在的文件existingfile创建新的符号链接，链接文件名为newfile.
 * 举例子说明：
 * 列出当前文件存在的信息，包括关键inode节点信息：
 * $ ls -il file1 file2
 * 10890912 -rw-rw-r-- 1  file1
 * 10890903 -rw-rw-r-- 1  file2
 * $ ln -s file1 symblink
 * $ ls -il file1 file2 symblink
 * 10890912 -rw-rw-r-- 2  file1
 * 10890903 -rw-rw-r-- 1  file2
 * 10890907 lrwxrwxrwx 1  symblink -> file1
 * $ cat file1 symblink
 * Hello, this is the file1.
 * Hello, this is the file1.
 * $ rm file1
 * $ ls -il file2 symblink
 * 10890903 -rw-rw-r-- 1  file2
 * 10890907 lrwxrwxrwx 1  symblink -> file1
 * $ cat symblink
 * cat: symblink: No such file or directory
 */
/**********
 * 函数功能：创建一个符号链接。
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
int symblink(const char *actualpath, const char *sympath);
/*********
 * 说明
 * 该函数创建一个指向actualpath的新目录项sympath，创建符号链接时，并不要求
 * actualpath存在，也可以位于不同的文件系统。
 */
/******************
 * 函数功能：打开符号链接
 * 返回值：若成功则返回以读到的字节数，若出错则返回-1；
 * 函数原型：
 */
ssize_t readlink(const char *pathname, char *buf, size_t bufsize);
