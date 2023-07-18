/*************************************************************************
    > File Name: pingpong.c
    > Author: savior
    > Created Time: 六  7/15 10:10:01 2023
 ************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main()
{
	int fd1[2],fd2[2];
    pipe(fd1);
    pipe(fd2);
    int pid=fork();
    if(pid==0)//子进程
    {
        close(fd1[1]);
        close(fd2[0]);
        char c;
        read(fd1[0],&c,1);
        printf("%d: received ping\n",getpid());
        write(fd2[1],&c,1);
        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    }else//父进程
    {
        close(fd1[0]);
        close(fd2[1]);
        char c='h';
        write(fd1[1],&c,1);
        read(fd2[0],&c,1);
        printf("%d: received pong\n",getpid());
        close(fd1[1]);
        close(fd2[0]);
        exit(0);
    }
	return 0;
}
