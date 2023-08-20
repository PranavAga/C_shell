#include "headers.h"

// Extract command properties and type
// 0: foreground; 1: background
int runcmd(int type,char* input, char* launch_dir,char*prev_dir){
    char*cmdtoken=strtok(input," ");
    if(strcmp(WARP,cmdtoken)==0){
        return warp(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
    }
    else if(strcmp(PEEK,cmdtoken)==0){
        return peek(cmdtoken+strlen(cmdtoken)+1,launch_dir,prev_dir);
    }
    else{
        cmderror(cmdtoken,"is not a valid command");
        return -1;
    }
}