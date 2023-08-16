#include "headers.h"

//shell prompt of the form <Username@SystemName:CurrDir> when waiting for user input
void prompt() {
    //UserID
    int userID=getuid();

    char sysname[MAX_HOSTNAME+1];

    // password file of the user
    struct passwd *pwd_file=getpwuid(userID);
    
    // SystemName(hostname)
    if(gethostname(sysname,MAX_HOSTNAME+1)){
        pcerror("Error in getting hostname");
    }
    //shell prompt
    else{
        printf("<"
        GREEN"%s"
        DEFAULT"@"
        BLUE"%s"
        DEFAULT":"
        WHITE"%s"
        DEFAULT">",
        pwd_file->pw_name,sysname,"CurrDir");
    }
}
