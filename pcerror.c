#include "headers.h"

// prints colored error message
void pcerror(char*msg){
    perror(RED"Error in getting hostname"BOLD_RED);
    printf(DEFAULT);
    return;
}