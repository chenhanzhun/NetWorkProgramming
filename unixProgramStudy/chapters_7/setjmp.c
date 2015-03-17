/* setjmp 和 longjmp 函数 */

/*
 * 函数功能：当某个嵌套函数出错时，可实现程序控制流跳转；
 * 函数原型：
 * #include <setjmp.h>
 * int setjmp(jmp_buf env);//若直接调用则返回0，若从longjmp调用则返回非0值；
 *
 * void longjmp(jmp_buf env, int val);
 * 说明：
 * 参数env是某种形式的数组，存放在调用longjmp时能恢复栈状态的所有信息，一般定义为全局变量；
 * val设置setjim的返回值；
 */
