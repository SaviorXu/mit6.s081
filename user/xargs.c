#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
int main(int argc,char *argv[])
{
    //命令行第一个参数为所要执行的命令，接下来的参数为执行命令所需的参数
    //从标准输入中读取每行数据。一个字节一个字节读。分割提取参数，这些参数为执行命令所需的附加参数。
    //每一行对应一个子进程，子进程执行其命令。 
    //注意parameter每一维的长度不能太长（1024），否则出现trap
    char parameter[MAXARG][100];
    char *res[MAXARG];
    while(1)
    {
        int num=0;
        char c;
        int word_num=0;
        memset(parameter,0,sizeof(parameter));
        for(int i=1;i<argc;i++)
        {
            strcpy(parameter[num++],argv[i]);
        }
        int ret;
        while((ret=read(0,&c,sizeof(char)))>0)
        {
            if(c==' ')
            {
                word_num=0;
            }else if(c=='\n')
            {
                num++;
                break;
            }else
            {
                parameter[num][word_num++]=c;
            }
        }
        if(ret<=0)
        {
            break;
        }
        for(int i=0;i<num;i++)
        {
            res[i]=parameter[i];
        }
        if(fork()>0)
        {
            int exit_status;
            wait(&exit_status);
        }else
        {
            exec(argv[1],res);
            exit(0);    
        }
          
    }
    exit(0); 
}