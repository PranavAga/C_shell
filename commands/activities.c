#include "../headers.h"

/*  activities  */

int lexico_int(const void *a,const void *b){
    return *(int*)a-*(int*)b;
}
int numdig(int num){
    if(num<10 &&num>=0){
        return 1;
    }
    else if(num>=0){
        return numdig(num/10)+1;
    }
}
char*int2str(int num){
    if(num<0){
        return NULL;
    }
    int ndig=numdig(num);
    int dig=num%10;
    int ntemp=num/10;
    
    char*str=malloc(ndig+1);
    for(int i=ndig-1;i>=0;i--){
        str[i]= dig+'0';
        dig=ntemp%10;
        ntemp/=10;
    }
    str[ndig]='\0';
    
    return str;
}

char*pname(Pnode all,pid_t pid){
    Pnode temp=all;
    while (temp)
    {
        if(temp->pid==pid){
            return temp->name;
        }
        temp=temp->next;
    }
    return "COMMAND NOT FOUND\0";
} 

// Lists all the processes that were spawned by the shell in lexicographic order
int activities( Pnode allpheadptr){
    int np=0;
    Pnode temp=allpheadptr;
    while (temp)
    {
        temp=temp->next;
        np++;
    }

    int pids[np];
    temp=allpheadptr;
    int i=0;
    while (temp)
    {
        pids[i]=temp->pid;
        temp=temp->next;
        i++;
    }

    qsort(pids,np,sizeof(int),lexico_int);

    char status[2];
    int _1;
    char _2[17];
    // [pid] : [command name] - [State]
    for(i=0;i<np;i++){
        printf("%d : ",pids[i]);
        printf("%s - ",pname(allpheadptr,pids[i]));

        char*pidstr=int2str(pids[i]);
        char procstat[6+strlen(pidstr)+7];
        strcpy(procstat,"/proc/");
        strcat(procstat,pidstr);
        strcat(procstat,"/stat");

        FILE*statp=fopen(procstat,"r");
        if(statp==NULL){
            if(errno==ENOENT){
                printf("Stopped\n");
                continue;
            }
            pcerror("Couldn't open process's stat file");
            return -1;
        }
        fscanf(statp,"%d %s %s",&_1,_2,status);
        fclose(statp);
        free(pidstr);
        if(status[0]!='T'){
            printf("Running\n");
        }
        else{
            printf("Stopped\n");
        }
        
    }
    return 0;
    
}