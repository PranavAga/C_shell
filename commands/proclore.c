#include "../headers.h"

/*  proclore */

void pid2str(char* pidstr,pid_t pid){
    pid_t n = pid;
    int len=0;
    while (n)
    {
        n /= 10;
        len++;
    }
    int dig;
    n = pid;
    for (int i = 0; i < len; i++){
        dig = n % 10;
        n/=10;
        pidstr[len-(i+1)]=dig+'0';
    }
    pidstr[len] = '\0';
}

// Prints information about a process
int proclore(char*args,pid_t shellp,char*launch_dir){
    /**info for a proc: /proc/<pid>
     * pid : /stat(1)
     * process status : /stat(3) (8)==(5)
     * Process Group : /stat(5)
     * Virtual memory : /stat(23)
     * executable path : /exe
    */
    char pidstr[10];

    char*newpid=strtok(args," ");
    if(newpid==NULL|| strlen(newpid)==0){// shell's proc
        pid2str(pidstr,shellp);
        }
    else{
        strcpy(pidstr,newpid);
    }

    char status[2];
    int pgrp;
    int tpgid;
    unsigned long vsize;
    char exepath[MAX_PATH+1]="\0";

    int _1;
    char _2[17];
    int _4,_6,_7,_9,_10,_11,_12,_13,_14;
    long _16,_17,_18,_19,_20,_21,_15;
    unsigned long long _22;

    
    char procpid[6+strlen(pidstr)+1];
    strcpy(procpid,"/proc/");
    strcat(procpid,pidstr);

    char procstat[strlen(procpid)+7];
    strcpy(procstat,procpid);
    strcat(procstat,"/stat");

    char procexec[strlen(procpid)+7];
    strcpy(procexec,procpid);
    strcat(procexec,"/exe");

    FILE*statp=fopen(procstat,"r");
    if(statp==NULL){
        pcerror("Couldn't open process's stat file");
        return -1;
    }
    fscanf(statp,"%d %s %s %d %d %d %d %d %d %d %d %d %d %d %ld %ld %ld %ld %ld %ld %llu %lu",
    &_1,_2,status,&_4,&pgrp,&_6,&_7,&tpgid,&_9,&_10,&_11,&_12,&_13,&_14,&_15,&_16,&_17,&_19,&_20,&_21,&_22,&vsize);
    
    fclose(statp);

    printf("pid : %d\n",_1);
    printf("process status : %s",status);
    if(pgrp==tpgid){// foreground
        printf("+\n");
    }
    else{
        printf("\n");
    }
    printf("Process Group : %d\n",pgrp);
    printf("Virtual memory : %lu\n",vsize);

    if(readlink(procexec,exepath,MAX_PATH)<-1){
        pcerror("Couldn't find executable path");
        return -1;
    }
    if(strstr(exepath,launch_dir)){
        char*relfile=exepath+strlen(launch_dir)+1;
        strcpy(exepath,"~/");
        strcat(exepath,relfile);
    }
    printf("executable path : %s\n",exepath);
    strcpy(exepath,"");
    return 0;
}