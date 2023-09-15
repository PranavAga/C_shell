#include "headers.h"

// Extract command properties and type
// 0: foreground; 1: background
int runcmd(int type,char* input,
    char* launch_dir,char*curr_dir,char* prev_dir,
    Pnode* bpheadptr, Pnode* allpheadptr,char*longcmd,time_t*timediff,pid_t shellp, 
    int is_ipipe,int is_opipe,char* i_file,char* o_file,int* ipipe,int* opipe, int oappend)
{
    // Backup of stdio, stdin
    int cpy_stdin=dup(STDIN_FILENO);
    int cpy_stdout=dup(STDOUT_FILENO);
    int offd;
    int iffd;

    if (is_ipipe&&!strlen(i_file)){
        if(dup2(ipipe[0],STDIN_FILENO)<0){
            pcerror("duplicating 2 STDIN");
            return -1;
        };
        close(ipipe[0]);
    }
    else if(strlen(i_file)){
        char effective_path[MAX_PATH+1];
        if(i_file[0]=='~'){
            tilde2abs(i_file,launch_dir,effective_path);
        }
        else{
            strcpy(effective_path,i_file);
        }
        
        iffd=open(effective_path,O_RDONLY);
        if(iffd<0){
            if(errno==ENOENT){
                cerror("No such input file found!");
            }
            else{
                pcerror("Input file file");
            }
            return -1;
        };
        if(dup2(iffd,STDIN_FILENO)<0){
            pcerror("duplicating 2 STDIN");
        };
    }
    if(is_opipe&&!strlen(o_file)){
        if(dup2(opipe[1],STDOUT_FILENO)<0){
            pcerror("duplicating 2 STDOUT");
            return -1;
        };
        close(opipe[1]);
    }
    else if(strlen(o_file)){
        // Handle file path: "..", "." and / handled by open()
        char effective_path[MAX_PATH+1];
        if(o_file[0]=='~'){
            tilde2abs(o_file,launch_dir,effective_path);
        }
        else{
            strcpy(effective_path,o_file);
        }
        if(oappend){
            offd=open(effective_path,O_APPEND|O_CREAT|O_WRONLY,OUT_PERMS);
        }
        else{
            offd=open(effective_path,O_CREAT|O_TRUNC|O_WRONLY,OUT_PERMS);
        }
        if(offd<0){
            pcerror("Output file");
            return -1;
        };
        if(dup2(offd,STDOUT_FILENO)<0){
            pcerror("duplicating 2 STDOUT");
            return -1;
        };
    }

    char*cmdtoken=strtok(input," ");
    if(strcmp(WARP,cmdtoken)==0){
        return warp(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
    }
    else if(strcmp(PEEK,cmdtoken)==0){
        return peek(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
    }
    else if(strcmp(PROCLORE,cmdtoken)==0){
        return proclore(strtok(NULL," "), shellp,launch_dir);
    }
    else if(strcmp(SEEK,cmdtoken)==0){
        return seek(cmdtoken+strlen(cmdtoken)+1,curr_dir,prev_dir);
    }
    else if(strcmp(ACTIVITIES,cmdtoken)==0){
        return activities(*allpheadptr);
    }
    else if(strcmp(IMAN,cmdtoken)==0){
        return iMan(strtok(NULL," "));
    }
    else if(strcmp(PING,cmdtoken)==0){
        return ping(cmdtoken+strlen(cmdtoken)+1);
    }
    else if(strcmp(FG,cmdtoken)==0){
        return fg(cmdtoken+strlen(cmdtoken)+1,shellp);
    }
    else if(strcmp(BG,cmdtoken)==0){
        return bg(cmdtoken+strlen(cmdtoken)+1);
    }
    else if(strcmp(NEONATE,cmdtoken)==0){
        // TODO: neonate Q236
        return neonate(cmdtoken+strlen(cmdtoken)+1);
    }
    else{
        time_t begin,end;
        char *list [MAX_INP/2];
        list[0]=cmdtoken;
        int i=1;
        char*cmdargs=cmdtoken+strlen(cmdtoken)+1;
        char*argptr=NULL;
        cmdargs=__strtok_r(cmdargs," ",&argptr);
        while (cmdargs)
        {
            list[i]=cmdargs;
            cmdargs=__strtok_r(NULL," ",&argptr);
            i++;
        }
        list[i]=NULL;
        int fr=fork();
        begin=time(NULL);
        if(fr==0){
            // Handle group process
            if(type){
                if(setpgid(0,0)<0){
                    pcerror("Settign gid of a background process");
                }
            }
            if(type==0){
                __sighandler_t resetCC = signal(SIGINT,SIG_DFL);
                if(resetCC==SIG_ERR){
                    pcerror("Couldn't reset Ctrl+C 's signal");
                }
            }
            int sysret=execvp(cmdtoken,list);
            
            if(sysret){
                if(errno=135){
                    cmderror(cmdtoken,"is not a valid command");
                }else{
                    pcerror("Problem with running the command");
                }
                exit(1);
            }
        }
        else if(fr>0){
            if(type){
                printf("%d\n",fr);
                *bpheadptr=addbpid(*bpheadptr,fr,cmdtoken);
                // // fflush(stdout);
                // // Printing pipe's content
                // close(pipefd[1]);
                // char pbuff[MAX_INP];
                // int readbytes=read(pipefd[0],pbuff,MAX_INP-1);
                // while (readbytes)
                // {
                //     printf("%.*s", readbytes, pbuff);
                //     readbytes=read(pipefd[0],pbuff,MAX_INP-1);
                // }
                strcpy(longcmd,"");
                *timediff=0;
            }
            if(type==0){
                wait(NULL);
                end=time(NULL);
                if(difftime(end,begin)>FP_TIMELMT){
                    strcpy(longcmd,cmdtoken);
                    *timediff=difftime(end,begin);
                }
                else{
                    strcpy(longcmd,"");
                    *timediff=0;
                }
            }
            *allpheadptr=addbpid(*allpheadptr,fr,cmdtoken);
        }
        else{
            cerror("Couldn't create child process");
            return -1;
        }
        
    }

    // Restoring stdin, stdout and other fds      
    if (is_ipipe||strlen(i_file)){
        if(dup2(cpy_stdin,STDIN_FILENO)<0){
            pcerror("Restoring stdin");
            return -1;
        }
        if(strlen(i_file)){
            close(iffd);
        }
    } 
    if(is_opipe||strlen(o_file)){
        if(dup2(cpy_stdout,STDOUT_FILENO)<0){
            pcerror("Restoring stdout");
            return -1;
        }
        if(strlen(o_file)){
            close(offd);
        }
    }
    
    return 0;
}

// Concat executed command(s) to add to pastevents
void add2executed(int type,char*curr_cmd,char*executed){
    if(curr_cmd==NULL || strlen(curr_cmd)==0){
        cerror("Empty command cann't be stored in history\n");
        return;
    }
    if(strstr(curr_cmd,PASTEVENTS)){
        strcat(executed,PASTEVENTS);
    }
    else{
        if(strlen(executed)!=0&&executed[strlen(executed)-1]!='&'){
            strncat(executed,MULTI_COMMANDS,strlen(MULTI_COMMANDS));
        }
        if(strlen(executed)==0){
            strncpy(executed,curr_cmd,strlen(curr_cmd));
        }
        else{
            strncat(executed,curr_cmd,strlen(curr_cmd));
        }
        if(type){
            strncat(executed,BACKG_P,strlen(BACKG_P));
        }
    }
}

// Add a command to pastevents
void storeevent(char*launch_dir,char* cmd){
    if(launch_dir==NULL||strlen(launch_dir)==0||cmd==NULL||strlen(cmd)==0){
        cerror("Invalid arguments for the function 'storeevent'");
        return;
    }
    if(strstr(cmd,PASTEVENTS)){
        return;
    } 

    char filepath[MAX_PATH+1]={0};
    strncpy(filepath,launch_dir,strlen(launch_dir));
    strcat(filepath,"/.");
    strcat(filepath,PASTEVENTS);

    FILE*history=fopen(filepath,"a+");
    if(history==NULL){
        perror("Opening 'pastevents' file");
        return;
    }
    
    // getting number of rows
    fseek(history,0,SEEK_SET);
    int num_commands=1;
    char chr=fgetc(history);
    if(chr==EOF){
        num_commands=0;
    }
    while (chr!=EOF)
    {
        if(chr=='\n'){
            num_commands++;
        }
        chr=fgetc(history);
    }

    //to check if last cmd is same
    if(num_commands){
        char latestcmd[MAX_INP];
        fseek(history,0,SEEK_SET);
        for(int t=0;t<num_commands;t++){
            fgets(latestcmd,MAX_INP,history);
        }
        if(strcmp(cmd,latestcmd)==0){
            return;
        }
    }
    
    // removing older events
    if(num_commands&&num_commands>=MAX_INHIST){
        int num_remove=num_commands-MAX_INHIST+1;
        int line_pos=0;
        char tmpstr[MAX_INP];

        //traverse till the command to be kept
        fseek(history,0,SEEK_SET);
        for(int t=0;t<num_remove;t++){
            fgets(tmpstr,MAX_INP,history);
        }

        char temppath[MAX_PATH+1]={0};
        strncpy(temppath,launch_dir,strlen(launch_dir));
        strcat(temppath,"/");
        strcat(temppath,"tmphist");

        FILE*temphist=fopen(temppath,"a+");
        if(temphist==NULL){
            perror("Opening 'temp' file");
            return;
        }
        char histchr=fgetc(history);
        while (histchr!=EOF)
        {
            fputc(histchr,temphist);
            histchr=getc(history);
        }
        fclose(history);
        fclose(temphist);
        remove(filepath);
        rename(temppath,filepath);
        history=fopen(filepath,"a+");
    }

    //store the command
    fseek(history,0,SEEK_END);
    if(num_commands!=0){
        fputc('\n',history);
    }
    for(int i=0;i<strlen(cmd);i++){
        if(cmd[i]!='\n'){
            fputc(cmd[i],history);
        }
    }
    fclose(history);
}

// Kill all running spawned processes
void killallp(Pnode allphead){
    Pnode temp=allphead;

    while (temp){
        kill(temp->pid,SIGKILL);
        temp=temp->next;
    }
    return;
}