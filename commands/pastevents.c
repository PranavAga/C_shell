#include "../headers.h"

/*  pastevents  */

// Manage 'pastevents' command
char*pastevents(char*launch_dir,char*cmd){
    char new_cmd[MAX_INP]={ 0 };
    char filepath[MAX_PATH+1]={0};
    strncpy(filepath,launch_dir,strlen(launch_dir));
    strcat(filepath,"/.");
    strcat(filepath,PASTEVENTS);

    if(strcmp(cmd,PASTEVENTS)==0){
        FILE*history=fopen(filepath,"a+");
        if(history==NULL){
            if(errno==EACCES){
                return NULL;
            }
            perror("Opening '.pastevents' file");
            return NULL;
        }
        fseek(history,0,SEEK_SET);
        char c=fgetc(history);
        if(c!=EOF){
            while (c!=EOF)
            {
                printf("%c",c);
                c=fgetc(history);
            }
            printf("\n");
        }

        strcpy(new_cmd,PASTEVENTS);
    }
    else if(strcmp(cmd,PAST_PURGE)==0){
        FILE*history=fopen(filepath,"w");
        if(history==NULL){
            perror("Opening 'pastevents' file");
            return NULL;
        }
        fclose(history);

        strcpy(new_cmd,PASTEVENTS);
    }
    else if(strlen(cmd)>(strlen(PAST_EXEC)+1)&&strncmp(cmd,PAST_EXEC,strlen(PAST_EXEC))==0){
        char*cmd_token=strtok(cmd," ");
        cmd_token=strtok(NULL," ");
        cmd_token=strtok(NULL," ");
        if(cmd_token==NULL){
            cmderror("pastevents execute",": Invalid arguments");
            return NULL;
        }

        int idx=0;
        for(int i=0;i<strlen(cmd_token);i++){
            if(cmd_token[i]<58&&cmd_token[i]>47){
                idx=10*idx+(cmd_token[i]-48);
            }
            else{
                cmderror("pastevents execute:","invalid arguments");
                return NULL;
            }
        }
        if(idx<=0||idx>MAX_INHIST){
            cmderror("pastevents execute:","index out of bound");
            return NULL;
        }

        FILE*history=fopen(filepath,"r");
        if(history==NULL){
            perror("Opening 'pastevents' file");
            return NULL;
        }
        fseek(history,0,SEEK_SET);
        int num_commands=1;
        char chr=fgetc(history);
        if(chr==EOF){
            num_commands=0;
        }
        while (chr!=EOF)
        {
            if(chr=='\n'){
                num_commands++;
            }
            chr=fgetc(history);
        }
        if(num_commands<idx){
            cmderror("pastevents execute",": not enough commands in past");
            return NULL;
        }

        char tmpstr[MAX_INP];
        //traverse till the command to executed and store it
        fseek(history,0,SEEK_SET);
        for(int t=0;t<num_commands-idx+1;t++){
            fgets(tmpstr,MAX_INP,history);
        }
        strcpy(new_cmd,tmpstr);
    }
    else{
        cmderror("pastevents",": invalid arguments");
        return NULL;
    }
    
    // Checking for '\n'
    if(new_cmd[strlen(new_cmd)-1]=='\n'){
        new_cmd[strlen(new_cmd)-1]='\0';
    }
    return strlen(new_cmd)>0?new_cmd:NULL;
}
