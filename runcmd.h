#ifndef __RUNCMD_H
#define __RUNCMD_H

#define WARP "warp"
#define PEEK "peek"
#define PASTEVENTS "pastevents"
#define PROCLORE "proclore"

int runcmd(int type,char* input,
    char* launch_dir,char* prev_dir,
    int*pipefd,Pnode* bpheadptr,char*longcmd,time_t*timediff
    ,pid_t shellp);
void add2executed(int type,char*curr_cmd,char*executed);
void storeevent(char*launch_dir,char* cmd);

#endif