#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
int main()
{
    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);
    if(fork()>0)//父进程
    {
        close(fd1[0]);
        close(fd2[1]);
        char w='f';
        write(fd1[1],&w,sizeof(w));
        read(fd2[0],&w,sizeof(w));
        printf("%d: received pong\n",getpid());
        close(fd1[1]);//若不关闭管道，会出bug
        close(fd2[0]);
        exit(0);
    }
    char r;
    close(fd1[1]);
    close(fd2[0]);
    read(fd1[0],&r,sizeof(r));
    printf("%d: received ping\n",getpid());
    write(fd2[1],&r,sizeof(r));
    close(fd1[0]);
    close(fd2[1]);
    exit(0);
}