#include <pwd.h>
/* The passwd structure.  */
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Password.  */
  uid_t pw_uid;		    /* User ID.  */
  gid_t pw_gid;		    /* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};

/****
 函数功能：获取口令文件信息
 返回值：若成功返回指针，出错则返回NULL；
 函数原型：
*****/
/* Search for an entry with a matching user ID. */
struct passwd *getpwuid (uid_t uid);

/* Search for an entry with a matching username. */
struct passwd *getpwnam (const char *name);


/****
 * 函数功能：查看整个口令文件信息；
 * 返回值：若成功则返回指针，若出错或达到文件尾则返回NULL；
 * 函数原型：
 * */
/* Read an entry from the password-file stream, opening it if necessary. */
 struct passwd *getpwent (void);

/* Rewind the password-file stream. */
 void setpwent (void);

/* Close the password-file stream. */
 void endpwent (void);

