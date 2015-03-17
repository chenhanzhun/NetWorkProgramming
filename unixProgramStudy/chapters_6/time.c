/* 时间和日期例程. */

/* time函数. */
/***
 * 函数功能：返回当前时间和日历。
 * 返回值：若成功则返回时间值，若出错则返回-1。
 * 函数原型：
 * **/
  #include <time.h>
  time_t time(time_t *calptr);
 /* 说明：
 * 若参数calptr非空，则返回的时间值存储在calptr所指的单元内；
 * ***/


/* tm structure.  */
struct tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/
};

/* 将秒转换为可读日期 */

/* localtime 和 gmtime 函数 */
/**
 * 函数功能：将时间秒转换为可读日期；
 * 返回值：返回指向tm结构的指针；
 * 函数原型：
 * Return the `struct tm' representation of *TIMER
   in Universal Coordinated Time (aka Greenwich Mean Time).*/ 
 struct tm *gmtime (const time_t *timer);

/* Return the `struct tm' representation
   of *TIMER in the local timezone.  */
 struct tm *localtime (const time_t *timer);
 /**
  * 区别：localtime返回的是当地时间；gmtime返回的是国际时间；
  * ***/

 /** 将可读时间转换为time_t时间 */
 time_t mktime(struct tm *tmptr);
 /* 返回值：若成功则返回日历时间，若出错则返回-1 */

 /* 以字符串显示时间的函数 */
 /**
  * 函数功能：以字符串显示的时间；
  * 返回值：指向以NULL结尾的字符串指针；
  * 函数原型：
  * */
/* Return a string of the form "Day Mon dd hh:mm:ss yyyy\n"
   that is the representation of TP in this format.  */
 char *asctime (const struct tm *tp);

/* Equivalent to `asctime (localtime (timer))'.  */
 char *ctime (const time_t *timer);

/* Format TP into buf according to FORMAT.
   Write no more than MAXSIZE characters and return the number
   of characters written, or 0 if it would exceed MAXSIZE.  */
 size_t strftime (char *restrict buf, size_t maxsize,
			const char *restrict format,
			const struct tm *restrict tp);
/**
 * 说明：
 * 时间tp以格式format形式存放在大小为maxsize的缓冲数组buf中。
 * 返回存在buf中字符数，不包括null字符；
 * **/
