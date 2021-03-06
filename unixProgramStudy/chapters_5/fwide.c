/***********
 * 设置流的定向
 * 返回值：
 *  1、若流是宽定向则返回正值；
 *  2、若流是字节定向则返回负值；
 *  3、若流是未定向则返回0；
 * 函数原型：
 * #include <stdio.h>
 * #include <wchar.h>
 * int fwide(FILE *fp, int mode);
 * 说明：根据mode的不同值，fwide操作不同：
 *  1、mode为负值，fwide试图使指定的流是字节定向；
 *  2、mode为正值，fwide试图使指定的流是宽定向；
 *  3、mode值为0，fwide不设置流的定向，返回标识该流定向的值；
 * 注意：fwide并不改变已定向流的定向
 * *************/

/************
 * 标准输入     stdin
 * 标准输出     stdout
 * 标准出错     stderr
 * ***********/

/**************
 * 缓冲：目的是减少数据的读写次数；
 * 标准I/O的缓冲类型：
 *  1、全缓冲：填满标准I/O缓冲区之后才进行实际I/O操作；
 *  2、行缓冲：挡在输入和输出中遇到换行符，则执行I/O操作；
 *  3、不带缓冲：标准I/O库不对字符进行缓冲存储；
 *
 * 函数功能：更改缓冲区类型
 * 返回值：若成功则返回0，若出错则返回非0；
 * 函数原型：
 * #include <stdio.h>
 * void setbuf(FILE *fp, char *buf);
 * int setvbuf(FILE *fp,char *buf, int mode, ssize_t size);
 * 注意：使用这两个函数必须确定fp流已经打开。
 *
 * setbuf函数打开或关闭缓冲机制，参数buf指定一个长度为BUFSIZE的缓冲区。
 * sevbuf函数根据参数mode指定缓冲类型：
 * mode参数：
 *      _IOFBF  全缓冲
 *      _IOLBF  行缓冲
 *      _IONBF  不带缓冲
 *____________________________________________________________
 |函数   | mode   | buf    | 缓冲区及长度       | 缓冲类型    |
 |_______|________|________|____________________|_____________|
 |setbuf |        | 非空   | 长度BUFSIZE的buf   | 全或行缓冲  |
 |       |        | NULL   | 无缓冲区           | 不带缓冲区  |
 |_______|________|________|____________________|_____________|
 |setvbuf| _IOFBF | 非空   | 长度size的buf      | 全缓冲      |
 |       |        | NULL   | 合适长度缓冲区     |             |
 |       | _IOLBF | 非空   | 长度size的buf      | 行缓冲      |
 |       |        | NULL   | 合适长度的缓冲区   |             |
 |       | _IONBF | 忽略   | 无缓冲区           | 不带缓冲区  |
 |_______|________|________|____________________|_____________|
 ********/
