#ifndef __COMM_H
#define __COMM_H

#define NO "-"
#define D "d"
#define R "r"
#define W "w"
#define X "x"

#define PAST_PURGE "pastevents purge"
#define PAST_EXEC "pastevents execute"

// commands' function
int warp(char*args,char* launch_dir,char*prev_dir);
int peek(char*args,char* launch_dir,char*prev_dir);
char*pastevents(char*launch_dir,char*cmd);
int proclore(char*args,pid_t shellp,char*launch_dir);
int seek(char*args,char* curr_dir,char*prev_dir);
int activities(Pnode allpheadptr);
int iMan(char* cmd);

// utility function
void tilde2abs(char*arg,char*launch_dir,char*newdir);
int hyph2abs(char* cmd,char*arg,char*prev_dir,char*newdir);

#endif