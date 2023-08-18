#include "headers.h"

int handlewrap(char*arg,char*launch_dir,char*prev_dir){
    char newdir[MAX_PATH+1];
    if (arg[0]=='~'){
        strcpy(newdir,launch_dir);
        size_t i=1;
        size_t ldir_size=strlen(launch_dir);
        while(i<strlen(arg)){
            newdir[ldir_size+i-1]=arg[i];
            i++;
        }
    }
    else if(arg[0]=='-'){
        if(strlen(arg)>1){
            cmderror("warp:","'-' used wrong");
            return -1;
        }
        if(strlen(prev_dir)==0){
            cmderror("warp:","no previous directory");
            return -1;
        }
        strcpy(newdir,prev_dir);
    }
    else{
        if(chdir(arg)){
            pcerror("warping");
            return -1;
        }
        return 0;
    }
    if(chdir(newdir)){
        pcerror("warping (~/-)");
        printf("%s\n",newdir);
        return -1;
    }
    return 0;
}

// changes the directory that the shell is currently in
int warp(char* args,char* launch_dir,char*prev_dir){
    if(strlen(args)==0){
        cmderror("warp","need arguments");
        return -1;
    }
    char curr_dir[MAX_PATH+1];
    int res=0;
    char*argtoken=strtok(args," ");
    while (argtoken)
    {
        if(getcwd(curr_dir,MAX_PATH+1)==NULL){
            pcerror("Getting current directory");
            return -1;
        }
        res=handlewrap(argtoken,launch_dir,prev_dir);
        if(res){
            return res;
        }
        strcpy(prev_dir,curr_dir);
        argtoken=strtok(NULL," ");
    }
    return res;
}