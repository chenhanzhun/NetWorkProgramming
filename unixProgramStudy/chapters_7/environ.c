/* 环境变量 */

/* 环境表 */

/*
 * 每个程序都会对应一张环境表，环境表是一个字符指针数组，
 * 其中每个指针包含一个以null结束的C字符串地址。全局变量environ包含了
 * 该指针数组的地址： extern char **environ;
 * 下图是环境表，包含3个C字符串，每个字符串结尾都显式的有一个null字符。
 */

/* 环境变量 */

/*
 * 环境字符串一般表示如下：
 * name = value;环境变量名，环境变量值
 *
 * 函数功能：获取环境变量值；
 * 返回值：指向与name关联的value的指针，若未找到则返回NULL；
 * 函数原型：
 * #include <stdlib.h>
 * char *getenv(const char *name);
 *
 * 函数功能：增加环境变量；
 * 返回值：若成功则返回0，若出错则返回非0；
 * 函数原型：
 * #include <stdlib.h>
 * int putenv(char *str);
 * 说明：
 * 把name = value的字符串存放在环境表中，如果name已存在，则先删除其原来的定义；
 *
 *
 * 函数功能：修改环境变量；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * #include <stdlib.h>
 * int setenv(const char *name, const char *value, int rewrite);
 * int unsetenv(const char *name);
 * 说明：
 * setenv将name设置为value值，若name已经存在，当rewrite为非0时，先删除其现有定义；
 * 若rewrite为0，则不删除其现有定义，即不修改name的字符变量；
 *
 * unsetenv删除name的定义；
 *
 */
