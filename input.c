#include "headers.h"

// Finds if a character is a space/tab
int isgap(char c){
    return c==TAB||c==SPACE;
}

// Shift all characters of the string from position p to left, by 1 unit
void shift_left(int p,char*str){
    size_t length=strlen(str);
    if(length==p-1||p<0){
        return;
    }
    for(int i=p;i<length-1;i++){
        str[i]=str[i+1];
    }
    //reducing the string length
    str[length-1]='\0';
    return;
}

//Removing extra spaces and tabs from the input
void remspaces(char*str){
    size_t i=0;
    
    if(strlen(str)==0){
        return;
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
    return;
}