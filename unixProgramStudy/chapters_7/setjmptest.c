#include <setjmp.h>
#include "apue.h"

static jmp_buf env;
static int globval;

static void fun1(int,int,int,int);
static void fun2(void);
int main(void)
{
    int autoval;
    register int registerval;
    volatile int volatileval;
    static int staticval;
    globval = 1; autoval = 2;
    registerval = 3; volatileval = 4;
    staticval = 5;
    if(setjmp(env) != 0)
    {
        printf("after longjmp:\n");
        printf("globval= %d, autoval= %d, registerval= %d,"
                "volatileval= %d, staticval= %d\n",
                globval,autoval,registerval,volatileval,staticval);
        printf("longjmp error\n");
        exit(0);
    }
    globval = 10; autoval = 11;
    registerval = 12; volatileval = 13;
    staticval = 14;
    fun1(autoval,registerval,volatileval,staticval);
    exit(0);
}
static
void fun1(int i, int j, int k, int l)
{
    printf("in funi():\n");
    printf("globval= %d, autoval= %d, registerval= %d,"
                "volatileval= %d, staticval= %d\n",
                globval,i,j,k,l);
    fun2();

}
static void fun2(void)
{
    longjmp(env,1);
}
