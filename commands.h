#ifndef __COMM_H
#define __COMM_H

#define NO "-"
#define D "d"
#define R "r"
#define W "w"
#define X "x"

#define PAST_PURGE "pastevents purge"
#define PAST_EXEC "pastevents execute"

int warp(char*args,char* launch_dir,char*prev_dir);
int peek(char*args,char* launch_dir,char*prev_dir);
char*pastevents(char*launch_dir,char*cmd);

#endif