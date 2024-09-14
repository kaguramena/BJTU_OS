#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define BUF_SIZE 1024

struct linux_dirent64 {
    ino64_t        d_ino;    // inode number
    off64_t        d_off;    // offset to next dirent
    unsigned short d_reclen; // length of this record
    unsigned char  d_type;   // file type
    char           d_name[]; // filename (null-terminated)
};

int _ls(int argc, char *argv[])
{
    
    int fd;
    int nread;
    char path[20] = ".";
    char buf[BUF_SIZE];
    
    if (argc > 1)
    {
        strcpy(path, argv[1]);
    }
     
    fd = open(path, __O_DIRECTORY | O_RDONLY);
    if (fd == -1)
    {
        char error[512];
        sprintf(error, "ls: cannot access '%s': No such file or directory", path);
        perror(error);
        return 1;
    }
    
    for (;;)
    {
       
        nread = syscall(SYS_getdents64, fd, buf, BUF_SIZE);
        if (nread == -1)
        {
            perror("getdents error!");
            return 1;
        }
        if (nread == 0)
        {
            break;
        }
        struct linux_dirent64 *d;
        for (int bpos = 0; bpos < nread;)
        {
            d = (struct linux_dirent64 *)(buf + bpos);

            if ((strcmp(d->d_name, ".")==0 )|| (strcmp(d->d_name, "..") == 0))
            {
                bpos += d->d_reclen;
                continue;
            }
            //char d_type = *(buf + bpos + d->d_reclen - 1);
            if (d -> d_type == DT_REG)
            {
                printf("%s  ", d->d_name);
            }
            if (d -> d_type == DT_DIR)
            {
                printf("%s/  ", d->d_name);
            }
            //printf("%s    ", d->d_name);
            bpos += d->d_reclen;
        }
    }
    printf("\n");
    return 0;
}

// int main(int argc, char* argv[])
// {
//     _ls(argc,argv);
// }