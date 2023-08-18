#include "headers.h"

#define MULTI_COMMANDS ";"
#define BACKG_P "&"

int main()
{
    char launch_dir[MAX_PATH+1];
    char prev_dir[MAX_PATH+1];
    prev_dir[0]='\0';

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
        
        char*semi_saveptr=NULL;
        char*bgp_saveptr=NULL;
        char*semitoken=__strtok_r(input,MULTI_COMMANDS,&semi_saveptr);
        while (semitoken){
            // retokenizing for background processes
            char*bgptoken=__strtok_r(semitoken,BACKG_P,&bgp_saveptr);
            while (bgptoken){
                char*curr_cmd=bgptoken;
                bgptoken=__strtok_r(NULL,BACKG_P,&bgp_saveptr);  
                
                // check command type then run
                int type=bgptoken?1:0;
        
                runcmd(type,curr_cmd,launch_dir,prev_dir);
            }

            // reinitializing token for multiple commands, ';' seperated
            semitoken=__strtok_r(NULL,MULTI_COMMANDS,&semi_saveptr);
        }
    }
}
