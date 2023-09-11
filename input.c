#include "headers.h"

// Finds if a character is a space/tab
int isgap(char c){
    return c==TAB||c==SPACE;
}

// Shift all characters of the string from position p to left, by 1 unit
void shift_left(size_t p,char*str){
    size_t length=strlen(str);
    if(p<0){
        cerror("Position can't be less than 0");
    }
    if(length==p-1){
        return;
    }
    for(int i=p;i<length-1;i++){
        str[i]=str[i+1];
    }
    //reducing the string length
    str[length-1]='\0';
    return;
}

//Removing extra spaces and tabs from '\n' terminating string input
int remspaces(char*str){
    if(str[strlen(str)-1]!='\n'){
        cerror("remspaces: '\\n' terminated string expected");
        return -1;
    }

    size_t i=0;
    
    if(strlen(str)==0){
        return 0;
    }
    while(str[i]!='\n'){
        if(isgap(str[i])){
            if(str[i]==TAB){
                str[i]=SPACE;
            }
            if(i==0||isgap(str[i+1])||str[i+1]=='\n'
            ||str[i+1]=='&'||(i>0&&str[i-1]=='&')
            ||str[i+1]==';'||(i>0&&str[i-1]==';')
            ||str[i+1]==PIPE_CMD[0]||(i>0&&str[i-1]==PIPE_CMD[0])
            // ||str[i+1]==OA_FILE[0]||(i>0&&str[i-1]==OA_FILE[0])
            // ||str[i+1]==OT_FILE[0]||(i>0&&str[i-1]==OT_FILE[0])
            ||str[i+1]==RD_FILE[0]||(i>0&&str[i-1]==RD_FILE[0])){
                shift_left(i,str);
            }
            else i++;
        }
        else i++;
    }
    str[i]='\0';
    return 0;
}

char* modified(char* launch_dir,char*curr_dir){
    size_t launch_len=strlen(launch_dir);
    size_t i=0;

    while (i<launch_len)
    {
        // outside the shell's directory
        if(launch_dir[i]!=curr_dir[i]){
            return curr_dir;
        }
        i++;
    }
    size_t eff_size=strlen(curr_dir)-launch_len+2;
    char *eff_dir=malloc(eff_size);
    for(size_t j=0;j<eff_size;j++){
        if (j==0){
            eff_dir[j]='~';
        }
        else if(j==eff_size-1){
            eff_dir[j]='\0';
        }
        else{
            eff_dir[j]=curr_dir[launch_len+j-1];
        }
    }
    return eff_dir;
}

char* effective_path(char* launch_dir,char*curr_dir){
    
    if(strcmp(launch_dir,curr_dir)){
        return strlen(curr_dir)<strlen(launch_dir)?curr_dir:modified(launch_dir,curr_dir);
    }
    else{
        return "~";
    }
}

struct pnode{
    pid_t pid;
    Pnode next;
    char* name;
};

// Store a newly created background process's info
Pnode addbpid(Pnode head, pid_t pid,char*name){
    Pnode new=malloc(sizeof(struct pnode));
    if(new==NULL){
        pcerror("Adding PID to the list:");
    }
    new->next=NULL;
    new->pid=pid;
    new->name=(char*)malloc(strlen(name)+1);
    strcpy(new->name,name);
    if(head!=NULL){
        Pnode temp=head;
        while (temp->next)
        {
            temp=temp->next;
        }
        temp->next=new;
        return head;
    }
    else{
        return new;
    }
}

// Check the completion status of a background process, and print accordingly
Pnode checkstatus(Pnode head){
    Pnode temp=head;
    Pnode tprev=NULL;
    Pnode newhead=head;
    while (temp)
    {
        int status;
        // FIXME: nano no exit
        pid_t wret=waitpid(temp->pid,&status,WNOHANG);
        if(wret){
            if(WIFEXITED(status)){
                printf("%s exited normaly (%d)\n",temp->name,temp->pid);
            }
            else{
                printf("%s exited abnormaly (%d): status: %d\n",temp->name,temp->pid,status);
            }
            //remove node
            if(temp==newhead){
                newhead=temp->next;
            }
            else{
                tprev->next=temp->next;
            }
            Pnode nxt=temp->next;
            free(temp->name);
            free(temp);
            temp=nxt;
        }
        else{
            tprev=temp;
            temp=temp->next;
        }
    }
    return newhead;
}

//gives start and stop pos of next I/O symbol
void file_pos(int pos[2] ,char* str_ptr){
    pos[0]=0;
    pos[1]=0;

    // only includes the I/O symbol
    if (strlen(str_ptr)<2){
        return;
    }
    // char*temp=str_ptr+1;
    str_ptr++;
    pos[0]++;
    pos[1]++;
    for(int i=0;i<strlen(str_ptr);i++){
        if(str_ptr[i]!=' '){
            if(str_ptr[i]==RD_FILE[0]||str_ptr[i]==OA_FILE[0]){
                pos[0]=0;
                pos[1]=0;
                return;
            }
            break;
        }
        pos[0]++;
        pos[1]++;
    }
    for(int i=pos[0];i<strlen(str_ptr);i++){
        if(str_ptr[i]==RD_FILE[0]||str_ptr[i]==OA_FILE[0] || str_ptr[i]==' '){
            break;
        }
        pos[1]++;
    }    
    return ;
}