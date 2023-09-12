#ifndef __RUNCMD_H
#define __RUNCMD_H

#define WARP "warp"
#define PEEK "peek"
#define PASTEVENTS "pastevents"
#define PROCLORE "proclore"
#define SEEK "seek"
#define ACTIVITIES "activities"

int runcmd(int type,char* input,
    char* launch_dir,char*curr_dir,char* prev_dir,
    Pnode* bpheadptr, Pnode* allpheadptr,char*longcmd,time_t*timediff,pid_t shellp, 
    int is_pipe,int is_opipe,char* i_file,char* o_file,int* ipipe,int* opipe, int oappend);
void add2executed(int type,char*curr_cmd,char*executed);
void storeevent(char*launch_dir,char* cmd);

#endif