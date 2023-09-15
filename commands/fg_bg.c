#include "../headers.h"

/*  fg  bg  */

int extract_pid(char*cmd,char*args){
    if(args==NULL||strlen(args)==0){
        cmderror(cmd,": Incomplete arguments");
        return -1;
    }
    char*str_pid=strtok(args," ");
    if(str_pid==NULL){
        cmderror(cmd,": Incomplete arguments");
        return -1;
    }
    int pid=str2pint(str_pid);
    if(pid<0){
        cmderror(cmd,": Invalid PID");
        return -1;
    }
    return pid;
}

// Brings the running or stopped background process with corresponding pid to foreground, handing it the control of terminal.
int fg(char*args,pid_t shellp){
    int pid=extract_pid("fg",args);
    if(pid<0){
        return pid;
    }

    signal(SIGTTIN,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    if(kill(pid,SIGCONT)){
        if(errno==ESRCH){
            cmderror("fg",": No such process found");
        }
        else{
            pcerror("Could not send 'SIGCONT'");
        }
        return -1;
    }
    
    tcsetpgrp(0,pid);
    setpgid(pid,shellp);
    int *status;
    waitpid(pid,status,0);

    tcsetpgrp(0,shellp);
    signal(SIGTTIN,SIG_DFL);
    signal(SIGTTOU,SIG_DFL);
    return 0;
}

// Changes the state of a stopped background process to running
int bg(char*args){
    int pid=extract_pid("bg",args);
    if(pid<0){
        return pid;
    }
    if(kill(pid,SIGCONT)){
        if(errno==ESRCH){
            cmderror("bg",": No such process found");
        }
        else{
            pcerror("Could not send 'SIGCONT'");
        }
        return -1;
    }
    return 0;
}