/*************************************************************************
    > File Name: find.c
    > Author: savior
    > Created Time: 六  7/15 10:23:20 2023
 ************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 
#include "kernel/fs.h"

void find(char *path,char *target)
{
    char buf[512],*p;
    int fd;
    struct stat st;
    struct dirent de;
    if((fd=open(path,0))<0)
    {
        printf("find:cannot open %s\n",path);
        return;
    }
    if(fstat(fd,&st)<0)
    {
        printf("find:cannot stat %s\n",path);
        close(fd);
        return;
    }
    if(st.type==T_DIR)
    {
        while(read(fd,&de,sizeof(de))==sizeof(de))
        {
            strcpy(buf,path);
            p=buf+strlen(buf);
            *p++='/';
            if(de.inum==0)
                continue;
            
            if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
            {
                continue;
            }
            memmove(p,de.name,strlen(de.name));
            p[strlen(de.name)]=0;
            if(stat(buf,&st)<0)
            {
                continue;
            }
            if(st.type==T_DIR)
            {
                find(buf,target);
            }else if(st.type==T_FILE&&strcmp(de.name,target)==0)
            {
                printf("%s\n",buf);
            }
        }
    }
    close(fd);
}
int main(int argc,char *argv[])
{
    if(argc<3)
    {
        printf("find error\n");
        exit(0);
    }
    find(argv[1],argv[2]);
    exit(0);
}
