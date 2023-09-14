#include "../headers.h"

/*  ping    */

// positive int string to int
int str2pint(char*str){
    int ndig=strlen(str);
    if(ndig==0){
        return -1;
    }
    int num=0;
    for(int i=0;i<ndig;i++){
        if(str[i]>='0'&&str[i]<='9'){
            num*=10;
            num+=str[i]-'0';
        }
        else{
            return -1;
        }
    }
    return num;
}

// Send signals to processes; ping <pid> <signal_number>
int ping(char* args){
    if(args==NULL||strlen(args)==0){
        cmderror("ping: ","Incomplete arguments");
        return -1;
    }

    char*str_pid=strtok(args," ");
    if(str_pid==NULL){
        cmderror("ping",": Incomplete arguments");
        return -1;
    }
    int pid=str2pint(str_pid);
    if(pid<0){
        cmderror("ping",": Invalid pid");
        return -1;
    }

    char*str_sig=strtok(NULL," ");
    if(str_sig==NULL){
        printf("sig: %s\n",str_sig);
        cmderror("ping",": Incomplete arguments");
        return -1;
    }
    int signal=str2pint(str_sig);
    if(signal<0){
        cmderror("ping",": Invalid signal");
        return -1;
    }

    int modsig=signal%32;
    if(kill(pid,modsig)){
        if(errno==ESRCH){
            cmderror("ping",": No such process found");
        }
        else{
            pcerror("Could not send signal");
        }
        return -1;
    }

    printf("Sent signal %d to process with pid %d\n",modsig,pid);
    return 0;
}