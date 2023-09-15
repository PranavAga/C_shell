#include "../headers.h"

/*  neonate */

// TERMINAL SETTING
/*____________________*/
void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*____________________*/

int kbhit (void){
  struct timeval tv = {0L, 0L};
  fd_set rdfs;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  return select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}

// Prints the PID of the most recently created process on the system, this PID will be printed every [time_arg] seconds until the key ‘x’ is pressed
int neonate(char* args){
    if(args==NULL||strlen(args)==0){
        cmderror(NEONATE,": Incomplete arguments");
        return -1;
    }
    char*nflag=strtok(args," ");
    if(nflag==NULL||strncmp(nflag,"-n",strlen("-n"))){
        cmderror(NEONATE,": Incomplete arguments");
        return -1;
    }
    
    char*str_n=strtok(NULL," ");
    if(str_n==NULL){
        cmderror(NEONATE,": Incomplete arguments");
        return -1;
    }
    int n=str2pint(str_n);
    if(n<0){
        cmderror(NEONATE,": Invalid time argument");
        return -1;
    }

    // raw mode
    char c;
    time_t ctime=time(NULL),prevptime=time(NULL);

    setbuf(stdout, NULL);
    
    enableRawMode();     
    while (1) {
        if(kbhit()){
            c=getchar();
            if(c==STOP_NEO){
                // disableRawMode();
                break;
            }
        }
        ctime=time(NULL);
        if(difftime(ctime,prevptime)>=n){
            FILE* avgfd=fopen("/proc/loadavg","r");
            char fbuff[1000];
            if(fread(fbuff,sizeof(char),1000,avgfd)==0){
                pcerror("Couldn't read /proc/loadavg");
                disableRawMode();
                return -1;
            }
            char*contptr;
            char*content=__strtok_r(fbuff," ",&contptr);
            for(int i=0;i<4;i++){
                content=__strtok_r(NULL," ",&contptr);
            }
            printf("%s",content);
            prevptime=ctime;
            fclose(avgfd);
        }
    }
    disableRawMode();
        
    exit_neo:   
    printf("\n");
    
    return 0;
}