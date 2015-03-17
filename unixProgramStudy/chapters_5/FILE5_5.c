/*********
 * 流操作
 *
 * 1、打开流
 *
 * 函数功能：打开一个标准I/O流；
 * 返回值：若成功返回文件指针，若出错返回NULL；
 * 函数原型：
 * #include <stdio.h>
 *
 * FILE *fopen(const char *pathname, const char *type);
 *
 * FILE *freopen(const char *pathname, const char *type, FILE *fp);
 *
 * FILE *fdopen(int filedes, const char *type);
 * 说明：
 * （1）fopen打开一个指定的文件pathname；
 * （2）freopen在指定的流fp上打开一个指定的文件pathname，若流已经打开，则先关闭该流；
 *      若该流已经定向，则freopen清除该流定向。
 * （3）fdopen获取一个现有的文件描述符，并使一个标准I/O流与该文件描述符相结合。
 *
 * 参数type指定对该I/O的读、写方式：
 * r或rb            为读而打开
 * w或wb            把文件截短至0，或为写而创建
 * a或ab            添加；为在文件尾写而打开，或为写而创建
 * r+或r+b或rb+     为读和写而打开
 * w+或w+b或wb+     把文件截短至0，或为读和写而打开
 * a+或a+b或ab+     为在文件尾读和写而打开或创建
 *
 *
 * 2、关闭一个已打开的流
 *
 * 函数功能：关闭一个已打开的流；
 * 返回值：若成功则返回0，若出错则返回EOF；
 * 函数原型：
 * #include <stdio.h>
 * int fclose(FILE *fp);
 * *********************/

/************
 * 读、写流
 * 可用三种非格式化I/O对已打开的流进行读写：
 *  （1）每次一个字符的I/O；一次读或写一个字符。
 *  （2）每次一行的I/O；可用fgets和fputs读写，每行以换行符终止。
 *  （3）直接I/O；可用fread和fwrite进行读写。
 * 1、每次一个字符的I/O操作
 *      输入函数
 *
 *      函数功能：读取一个字符；
 *      返回值：若成功返回下一个字符，若已到达文件尾或出错则返回EOF;
 *      函数原型：
 *      #include <stdio.h>
 *      int getc(FILE *fp);
 *      int fgetc(FILE *fp);
 *      int getchar(void);
 *      说明：
 *      函数getchar等价于getc(stdin)，前两个函数的区别是getc可被实现为宏，
 *      而fgetc不能，则有以下区别：
 *      （1）getc参数不能具有副作用的表达式；
 *      （2）允许fgetc的地址作为一个参数传递给其它函数；
 *      （3）调用fgetc所需时间比getc长，即函数调用时间长与宏调用；
 *
 *     函数功能： 出错或文件结束标志
 *     返回值：条件为真返回非0，否则返回0；
 *     函数原型：
 *     #include <stdio.h>
 *     int ferror(FILE *fp);
 *     int feof(FILE *fp);
 *
 *     void clearerr(FILE *fp);//清除出错和文件结束标志
 *
 *     输出函数
 *
 *     与输入函数对应有三个输出函数
 *     函数功能：输出(写)一个字符；
 *     返回值：若成功返回c，出错返回EOF；
 *     函数原型：
 *     #include <stdio.h>
 *     int putc(int c, FILE *fp);
 *     int fputc(int c, FILE *fp);
 *     int putchar(int c);
 *     说明：
 *     putchar等价于putc(c,stdout)。putc可实现为宏，而fputc不能。
 *
 * 2、每次一行的流I/O操作
 *
 *  输入函数
 *
 *  函数功能：每次输入一行字符
 *  返回值：若成功返回buf，若出错或已达到文件尾则返回NULL；
 *  函数原型：
 *  #include <stdio.h>
 *  char *fgets(cahr *buf, int n, FILE *fp);
 *  char *gets(char *buf, FILE *fp);
 *  说明：
 *  这两个函数都是将读入的行字符存储到缓冲区buf中。但是gets从标准输入读，
 *  fgets从指定的流fp读。
 *  参数：
 *  buf是缓冲区；
 *  n是指定缓冲区的长度；
 *  fp是文件流；
 *
 *  输出函数
 *
 *  函数功能：每次输出一行字符；
 *  返回值：若成功返回非负值，若出错则返回EOF；
 *  函数原型：
 *  #include <stdio.h>
 *  int fputs(const char *str, FILE *fp);
 *  int puts(const char *str);
 *
 *  3、二进制I/O
 *
 *  函数功能：读、写整个结构
 *  返回值：读或写的对象数
 *  函数原型：
 *  size_t fread(void *ptr, size_t size, size_t nobj, FILE *fp);
 *  size_t fwrite(void *ptr, size_t size, size_t nobj, FILE *fp);
 *  说明：
 *  ptr是读或写数据的起始位置；
 *  size是数据结构的sizeof大小；
 *  nobj是读或写的对象个数
 *  fp是文件流；
 *     *********/

 /*********
  * 定位流
  * 有三种方法定位流：
  * 1、ftell和fseek函数，他们假定文件的位置可存放在一个长整型中；
  * 2、ftello和fseeko函数，他们使用off_t数据类型代替长整型；
  * 3、fgetpos和fsetpos函数，他们使用一种抽象的数据类型fpos_t记录文件的位置；
  *
  * 函数原型：
  * #include <stdio.h>
  * long ftell(FILE *fp)//若成功返回当前文件位置的指示，出错返回-1L；
  * int fseek(FILE *fp, long offset, int whence);//若成功返回0，出错返回非0；
  * 说明:
  * 对于二进制文件，其文件位置指示器是从文件起始位置开始度量，并以字节为计量单位；
  * 参数：
  * offset是偏移量；
  * whence的值与lssek函数的参数值一样：SEEK_SET起始位置，SEEK_CUR当前位置，SEEK_END尾端；
  *
  * #include <stdio.h>
  * off_t ftello(FILE *fp);//若成功返回当前文件位置指示，出错返回-1
  * int fsseko(FILE *fp, off_t offset, int whence);//若成功则返回0，出错返回非0
  * 除了offset的类型是off_t而非long以外，ftello与ftell相同，fseeko与fseek相同；
  *
  *
  * int fgetpos(FILE *fp, fpos_t *pos);
  * int fsetpos(FILE *fp, const fpos_t *pos);
  * 返回值：两函数若成功则返回0，出错返回非0值；
  *
  * ****/
