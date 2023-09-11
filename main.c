#include "headers.h"

int main()
{
    char launch_dir[MAX_PATH+1];
    char prev_dir[MAX_PATH+1];
    char longcmd[MAX_INP];
    time_t runtime=0;
    prev_dir[0]='\0';
    Pnode bphead=NULL;

    if(getcwd(launch_dir,MAX_PATH+1)==NULL){
        pcerror("Getting launch directory");
        return -1;
    }

    pid_t shellp=getpid();

    // Keep accepting commands
    while (1)
    {
        char curr_dir[MAX_PATH+1];
        if(getcwd(curr_dir,MAX_PATH+1)==NULL){
            pcerror("Getting current directory");
            return -1;
        }

        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(effective_path(launch_dir,curr_dir),longcmd,runtime);
        char input[MAX_INP];
        fgets(input, MAX_INP, stdin);
        char executed[MAX_INP]={ 0 };
        if(remspaces(input)){
            // printf("\n");
            continue;
        };
        // resetting sys foreground process time
        runtime=0;
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
                type=(bgptoken==NULL)&&(forelast)?0:1;

                if(strlen(curr_cmd)>=strlen(PASTEVENTS)&&strncmp(curr_cmd,PASTEVENTS,strlen(PASTEVENTS))==0){
                    curr_cmd=pastevents(launch_dir,curr_cmd);
                }
                if(curr_cmd!=NULL){
                    add2executed(type,curr_cmd,executed);
                    if(strcmp(curr_cmd,PASTEVENTS)){
                        // Checking for pipes
                        if(curr_cmd[0]==PIPE_CMD[0] || curr_cmd[strlen(curr_cmd)-1]==PIPE_CMD[0] || strstr(curr_cmd,"||")){
                            cerror("Invalid use of pipe");
                            break;
                        }
                        char*io_saveptr=NULL;
                        char*iotoken=__strtok_r(curr_cmd,PIPE_CMD,&io_saveptr);
                        int is_opipe=0;
                        int is_ipipe=0;
                        int ipipe[2]={STDIN_FILENO,STDOUT_FILENO};
                        int opipe[2]={STDIN_FILENO,STDOUT_FILENO};

                        while(iotoken){
                            char*pcurr_cmd=iotoken;
                            iotoken=__strtok_r(NULL,PIPE_CMD,&io_saveptr);
                            
                            is_opipe=iotoken? 1:0;

                            if(is_ipipe){
                                ipipe[0]=opipe[0];
                                ipipe[1]=opipe[1];
                            }
                            if(is_opipe){
                                if(pipe(opipe)){
                                    // TODO: close pipes wherever
                                    pcerror("Couldn't initialize output pipe");
                                    return -1;
                                }
                            }
                            else{
                                opipe[0]=STDIN_FILENO;
                                opipe[1]=STDOUT_FILENO;
                            }
                            if (pcurr_cmd[0]==OT_FILE[0]||pcurr_cmd[0]==OA_FILE[0]||pcurr_cmd[0]==RD_FILE[0]){
                                cerror("Syntax Error for I/O Redirection");
                                break;
                            }

                            char io_pcmd[strlen(pcurr_cmd)+1];
                            char io_pcmd_cpy[strlen(pcurr_cmd)+1];
                            strcpy(io_pcmd,pcurr_cmd);
                            strcpy(io_pcmd_cpy,pcurr_cmd);

                            // Find location of the first I/O symbols
                            char*inp_ptr=strstr(io_pcmd_cpy,RD_FILE);
                            char*ot_ptr=strstr(io_pcmd_cpy,OT_FILE);
                            char*oa_ptr=strstr(io_pcmd_cpy,OA_FILE);
                            char*oup_ptr=oa_ptr;
                            if(ot_ptr!=NULL && oa_ptr!=NULL && ot_ptr<oa_ptr ){
                                oup_ptr=ot_ptr;
                            }
                            else if(ot_ptr!=NULL && oa_ptr==NULL){
                                oup_ptr=ot_ptr;
                            }

                            char i_file[MAX_PATH]="\0";
                            char o_file[MAX_PATH]="\0";
                            int oappend=0;

                            char*cmd_saveptr=NULL;
                            char*cmdtoken=__strtok_r(io_pcmd,"><",&cmd_saveptr);
                            
                            // to not inlcude the files in the same address series, for further implementation
                            char onlycmd[strlen(cmdtoken)+1];
                            strcpy(onlycmd,cmdtoken);
                            
                            // getting first input or output file
                            if(inp_ptr){
                                int nchars[2] ;
                                file_pos(nchars,inp_ptr);
                                if(nchars[1]==0){
                                    cerror("Expected a file name after '<'");
                                    break;
                                }
                                strncpy(i_file,inp_ptr+nchars[0],nchars[1]-nchars[0]+1);
                            }
                            if(oup_ptr){
                                if(oup_ptr==oa_ptr){
                                    oappend=1;
                                    oup_ptr++;
                                }
                                int nchars[2] ;
                                file_pos(nchars,oup_ptr);
                                if(nchars[1]==0){
                                    cerror("Expected a file name after '>'");
                                    break;
                                }
                                strncpy(o_file,oup_ptr+nchars[0],nchars[1]-nchars[0]+1);
                            }
                            // printf("|CMD: %s,I: %s, O: %s|\n",cmdtoken,i_file,o_file);
                            // printf("type = %d is_ipipe = %d is_opipe = %d\n",type,is_ipipe,is_opipe);
                            
                            runcmd(type,onlycmd,launch_dir,curr_dir,prev_dir,
                            &bphead,longcmd,&runtime, shellp,
                            is_ipipe,is_opipe,i_file,o_file,ipipe,opipe,oappend);
                            
                            is_ipipe=1;
                        }
                        
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
