#include "headers.h"

//shell prompt of the form <Username@SystemName:CurrDir(dir)> when waiting for user input
void prompt(char* dir) {
    //UserID
    int userID=getuid();
    char sysname[MAX_HOSTNAME+1];
    
    // password file of the user
    struct passwd *pwd_file=getpwuid(userID);
    if(pwd_file==NULL){
        pcerror("Getting password file");
    }
    // SystemName(hostname)
    else if(gethostname(sysname,MAX_HOSTNAME+1)){
        pcerror("Getting hostname");
    }
    //shell prompt
    else{
        printf("<"
        GREEN"%s"
        DEFAULT"@"
        BLUE"%s"
        DEFAULT":"
        WHITE"%s"
        DEFAULT"> ",
        pwd_file->pw_name,sysname,dir);
    }
}
