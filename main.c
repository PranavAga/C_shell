#include "headers.h"

#define MULTI_COMMANDS ";"
#define BACKG_P "&"

int main()
{
    char launch_dir[MAX_PATH+1];
    if(getcwd(launch_dir,MAX_PATH+1)==NULL){
        pcerror("Getting launch directory");
        return -1;
    }
    // Keep accepting commands
    while (1)
    {
        char curr_dir[MAX_PATH+1];
        if(getcwd(curr_dir,MAX_PATH+1)==NULL){
            pcerror("Getting current directory");
            return -1;
        }

        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(effective_path(launch_dir,curr_dir));
        char input[4096];
        fgets(input, 4096, stdin);
        if(remspaces(input)){
            continue;
        };
        
        char*semitoken=strtok(input,MULTI_COMMANDS);
        char*nxt_semitoken;//input next to handle
        while (semitoken){
            // +1 for '\0'
            nxt_semitoken=semitoken+strlen(semitoken)+1;

            // retokenizing for background processes
            char*bgptoken=strtok(semitoken,BACKG_P);
            while (bgptoken){
                char*curr_cmd=bgptoken;
                bgptoken=strtok(NULL,BACKG_P);  
                
                // check command type then run
                if(bgptoken){
                    printf("Background\t%s\n",curr_cmd);
                }
                else{
                    printf("Foreground\t%s\n",curr_cmd);
                }
            }

            // reinitializing token for multiple commands, ';' seperated
            semitoken=strtok(nxt_semitoken,MULTI_COMMANDS);
        }
    }
}
