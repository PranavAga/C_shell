#include "headers.h"

// Finds if a character is a space/tab
int isgap(char c){
    return c==TAB||c==SPACE;
}

// Shift all characters of the string from position p to left, by 1 unit
void shift_left(size_t p,char*str){
    size_t length=strlen(str);
    if(p<0){
        cerror("Position can't be less than 0");
    }
    if(length==p-1){
        return;
    }
    for(int i=p;i<length-1;i++){
        str[i]=str[i+1];
    }
    //reducing the string length
    str[length-1]='\0';
    return;
}

//Removing extra spaces and tabs from '\n' terminating string input
int remspaces(char*str){
    if(str[strlen(str)-1]!='\n'){
        cerror("remspaces: '\\n' terminated string expected");
        return -1;
    }

    size_t i=0;
    
    if(strlen(str)==0){
        return 0;
    }
    while(str[i]!='\n'){
        if(isgap(str[i])){
            if(str[i]==TAB){
                str[i]=SPACE;
            }
            if(i==0||isgap(str[i+1])||str[i+1]=='\n'){
                shift_left(i,str);
            }
            else i++;
        }
        else i++;
    }
    str[i]='\0';
    return 0;
}

char* modified(char* launch_dir,char*curr_dir){
    size_t launch_len=strlen(launch_dir);
    size_t i=0;

    while (i<launch_len)
    {
        // outside the shell's directory
        if(launch_dir[i]!=curr_dir[i]){
            return curr_dir;
        }
        i++;
    }
    size_t eff_size=strlen(curr_dir)-launch_len+2;
    char *eff_dir=malloc(eff_size);
    for(size_t j=0;j<eff_size;j++){
        if (j==0){
            eff_dir[j]='~';
        }
        else if(j==eff_size-1){
            eff_dir[j]='\0';
        }
        else{
            eff_dir[j]=curr_dir[launch_len+j-1];
        }
    }
    return eff_dir;
}

char* effective_path(char* launch_dir,char*curr_dir){
    
    if(strcmp(launch_dir,curr_dir)){
        return strlen(curr_dir)<strlen(launch_dir)?curr_dir:modified(launch_dir,curr_dir);
    }
    else{
        return "~";
    }
}

