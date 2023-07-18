/*************************************************************************
    > File Name: sleep.c
    > Author: savior
    > Created Time: 六  7/15 09:58:53 2023
 ************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main(int argc,char *argv[])
{
    if(argc==1)
    {
        fprintf(2,"usage:sleep\n");
        exit(1);
    }
    int time=atoi(argv[1]);
    sleep(time);
    exit(0);
}
