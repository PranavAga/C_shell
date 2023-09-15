#include "../headers.h"

/*  warp    */

void tilde2abs(char*arg,char*launch_dir,char*newdir){
    strcpy(newdir,launch_dir);
    size_t i=1;
    size_t ldir_size=strlen(launch_dir);
    while(i<strlen(arg)){
        newdir[ldir_size+i-1]=arg[i];
        i++;
    }
    newdir[ldir_size+i-1]='\0';
}

int hyph2abs(char* cmd,char*arg,char*prev_dir,char*newdir){
    if(strlen(arg)>1){
        cmderror(cmd,": '-' used wrong");
        return -1;
    }
    if(strlen(prev_dir)==0){
        cmderror(cmd,": no previous directory");
        return -1;
    }
    strcpy(newdir,prev_dir);
    return 0;
}

int handlewrap(char*arg,char*launch_dir,char*prev_dir){
    char newdir[MAX_PATH+1];
    if (arg[0]=='~'){
        tilde2abs(arg,launch_dir,newdir);
    }
    else if(arg[0]=='-'){
        if(hyph2abs(WARP,arg,prev_dir,newdir)){
            return -1;
        }
    }
    else{
        if(chdir(arg)){
            printf("'%s'\n",arg);
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
