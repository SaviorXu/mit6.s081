#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
void find(char *path,char *target)
{
    int fd;
    struct stat st;
    struct dirent de;
    if((fd=open(path,0))<0)
    {
        fprintf(2,"find:cannot open %s\n",path);
        return;
    }
    if(fstat(fd,&st)<0)
    {
        fprintf(2,"find:cannot stat %s\n",path);
    }
    while(read(fd,&de,sizeof(de))==sizeof(de))
    {
        if(de.inum==0)
            continue;
        char tmp_path[1024]="0";
        strcpy(tmp_path,path);
        strcpy(tmp_path+strlen(path),"/");
        strcpy(tmp_path+strlen(path)+1,de.name);
        struct stat file_stat;
        if(stat(tmp_path,&file_stat)<0)
        {
            printf("find:cannot stat %s\n",tmp_path);
            continue;
        }
        if(file_stat.type==T_DIR)
        {
            if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
            {
                continue;
            }
            find(tmp_path,target);
        }else if(strcmp(de.name,target)==0){
            printf("%s\n",tmp_path);

        }
    }
    close(fd);
    return;
}
int main(int argc,char *argv[])
{
    
    find(argv[1],argv[2]);
    return 0;
}