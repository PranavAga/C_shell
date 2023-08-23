#include "headers.h"

int main()
{
    char launch_dir[MAX_PATH+1];
    char prev_dir[MAX_PATH+1];
    prev_dir[0]='\0';
    Pnode bphead=NULL;
    int pipefd[2]; 
    if(pipe(pipefd)){
        pcerror("Couldn't initialize pipe");
        return -1;
    }

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
        char input[MAX_INP];
        fgets(input, MAX_INP, stdin);
        char executed[MAX_INP]={ 0 };
        if(remspaces(input)){
            continue;
        };
        bphead=checkstatus(bphead);
        char*semi_saveptr=NULL;
        char*semitoken=__strtok_r(input,MULTI_COMMANDS,&semi_saveptr);
        while (semitoken){
            if(strlen(semitoken)>0&&semitoken[0]=='&'){
                cerror("Syntax error using '&'");
                break;
            }
            // retokenizing for background processes
            char*bgp_saveptr=NULL;
            int type=1,forelast=1;
            if(semitoken[strlen(semitoken)-1]=='&'){
                forelast=0;
            }
            char*bgptoken=__strtok_r(semitoken,BACKG_P,&bgp_saveptr);
            while (bgptoken){
                char*curr_cmd=bgptoken;
                bgptoken=__strtok_r(NULL,BACKG_P,&bgp_saveptr);

                // check command type then run
                type=bgptoken==NULL&&forelast?0:1;

                if(strlen(curr_cmd)>=strlen(PASTEVENTS)&&strncmp(curr_cmd,PASTEVENTS,strlen(PASTEVENTS))==0){
                    curr_cmd=pastevents(launch_dir,curr_cmd);
                }
                if(curr_cmd!=NULL){
                    add2executed(type,curr_cmd,executed);
                    if(strcmp(curr_cmd,PASTEVENTS)){
                        runcmd(type,curr_cmd,launch_dir,prev_dir,pipefd,&bphead);
                    }
                }
                
            }

            // reinitializing token for multiple commands, ';' seperated
            semitoken=__strtok_r(NULL,MULTI_COMMANDS,&semi_saveptr);
        }
        // store in pastevents
        if(strlen(executed)){
            storeevent(launch_dir,executed);
        }        
    }
}
