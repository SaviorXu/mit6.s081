#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
int judge(int num,int num2)
{
    for(int i=2;i<num;i++)
    {
        if(num%num2==0)
        {
            return 0;
        }
    }
    return 1;
}
void exec_prime(int *p)
{
    close(p[1]);
    int num,st;
    if(read(p[0],&st,4)<=0)
    {
        return;
    }
    printf("prime %d\n",st);
    int p2[2];
    pipe(p2);
    if(fork()>0)
    {
        close(p2[0]);
        while(read(p[0],&num,4)>0)
        {
            if(judge(num,st)==1)
            {
                write(p2[1],&num,4);
            }
        }
        int exit_status;
        close(p[0]);
        close(p2[1]);
        wait(&exit_status);
    }else
    {
        exec_prime(p2);
    }
}
int main()
{
    int st=2;
    int p[2];
    pipe(p);
    printf("prime %d\n",st);
    if(fork()>0)
    {
        close(p[0]);
        for(int i=st+1;i<35;i++)
        {
            if(judge(i,st)==1)
            {
                write(p[1],&i,4);
            }
        }
        int exit_status;
        close(p[1]);
        wait(&exit_status);
    }else
    {
        exec_prime(p);
    }
    exit(0);
}
