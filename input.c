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
            if(i==0||isgap(str[i+1])||str[i+1]=='\n'){
                shift_left(i,str);
            }
            else i++;
        }
        else i++;
    }
    return 0;
}