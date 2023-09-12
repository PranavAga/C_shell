#ifndef __INPUT_H
#define __INPUT_H

#define TAB '\t'
#define SPACE ' '

struct pnode{
    pid_t pid;
    struct pnode* next;
    char* name;
};

typedef struct pnode *Pnode;

int isgap(char );
void shift_left(size_t,char*);
int remspaces(char*);
char* effective_path(char*,char*);
Pnode addbpid(Pnode head, pid_t pid,char*name);
Pnode checkstatus(Pnode head);
void file_pos(int pos[2] ,char* str_ptr);

#endif