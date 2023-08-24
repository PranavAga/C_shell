#include "headers.h"

// Extract command properties and type
// 0: foreground; 1: background
int runcmd(int type,char* input, char* launch_dir,char*prev_dir,int*pipefd,Pnode* bpheadptr,char*longcmd,time_t*timediff){
    char*cmdtoken=strtok(input," ");
    if(strcmp(WARP,cmdtoken)==0){
        return warp(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
    }
    else if(strcmp(PEEK,cmdtoken)==0){
        return peek(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
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
            if(type){
                    // //open pipe write
                    // //close(STDIN_FILENO);
                    // dup2(pipefd[1],STDOUT_FILENO);
                    // close(pipefd[1]);
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
            close(pipefd[1]);
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
            return 0;
        }
        else{
            cerror("Couldn't create child process");
            return -1;
        }
        
    }
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
    strcat(filepath,"/");
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