#include "headers.h"

// Prints colored perror message
void pcerror(char*title){
    printf(RED"ERROR"PURPLE"\n");
    perror(title);
    printf(DEFAULT);
    return;
}

// Prints colored error message
void cerror(char*msg){
    printf(RED"ERROR\n"PURPLE"%s\n"DEFAULT,msg);
};