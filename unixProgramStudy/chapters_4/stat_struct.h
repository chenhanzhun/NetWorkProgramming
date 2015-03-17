#include <sys/stat.h>
#include <unistd.h>

struct stat{
    mode_t      st_mode;    /* file type */
    ino_t       st_ino;     /* i-node number */
    dev_t       st_dev;     /* device number */
    dev_t       st_rdev;    /* device number for special file */
    nlink_t     st_nlink;   /* number of hard link */
    uid_t       st_uid;     /* user ID of owner */
    gid_t       st_gid;     /* group Id of owner */
    off_t       st_size;    /* total size in bytes for regular files */
    blksize_t   st_blksize; /* blocksize for filesystem I/O */
    blkcnt_t    st_blocks;  /* number of blocks allocated */
    time_t      st_atime;   /* time of last access */
    time_t      st_mtime;   /* time of last modification */
    time_t      st_ctime;   /* time of last change */
};
