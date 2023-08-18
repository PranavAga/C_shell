#ifndef __INPUT_H
#define __INPUT_H

#define TAB '\t'
#define SPACE ' '

int isgap(char );
void shift_left(size_t,char*);
int remspaces(char*);
char* effective_path(char*,char*);

#endif