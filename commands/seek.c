#include "../headers.h"

// get absolute path of a file in a directory
void rel2abs(char*abstarg_dir,char*targ_dir,char*curr_dir){
    if (targ_dir[0]=='~'){
        tilde2abs(targ_dir,curr_dir,abstarg_dir);
    }
    else if(targ_dir[0]=='.'&&strlen(targ_dir)>1&&targ_dir[1]=='/'){
        strcpy(abstarg_dir,curr_dir);
        strcat(abstarg_dir,targ_dir+1);
    }
    else if(targ_dir[0]!='/'){
        strcpy(abstarg_dir,curr_dir);
        strcat(abstarg_dir,"/");
        strcat(abstarg_dir,targ_dir);
    }
    else{
        strcpy(abstarg_dir,targ_dir);
    }
}

// Print all 'search' matching files/folders from a given dir, return number of matches
int printmatching(char*matchpath,int e,int of,int od,char* search,char*searching_dir,char*og_dir){
    DIR* dirp=opendir(searching_dir);
    if(dirp==NULL){
        if(errno==EACCES){
            cmderror("seek",": Missing permissions for task!");
            return 0;
        }
        else
        {
            pcerror("peek: Opening directory");
            return 0;    
        }
    }
    int i=0;
    struct dirent*entp=readdir(dirp);
    while (entp)
    {
        if(entp->d_name[0]=='.'){// if hidden
            entp=readdir(dirp);
            continue;
        }
        char*fname=malloc(strlen(entp->d_name)+1);
        strcpy(fname,entp->d_name);
        char*tok_fname=strtok(fname,".");

        struct stat fstat;
        char filepath[MAX_PATH+1];
        strcpy(filepath,searching_dir);
        strcat(filepath,"/");
        strcat(filepath,entp->d_name);
        if(stat(filepath,&fstat)){
            pcerror("seek: Getting stats of file");
            return -1;
        };

        if(S_ISDIR(fstat.st_mode)){
            //parse through the dir
            i+=printmatching(matchpath,e,of,od,search,filepath,og_dir);
            
            if(of||strcmp(tok_fname,search)){
                entp=readdir(dirp);
                continue;
            }
            printf(BLUE);
        }
        else{
            if(od||strcmp(tok_fname,search)){
                entp=readdir(dirp);
                continue;
            }
            printf(GREEN);
        }

        if(e){
            printf(DEFAULT);
            strcpy(matchpath,filepath);
        }
        else{
            printf(".%s"DEFAULT"\n",filepath+strlen(og_dir));
        }
        entp=readdir(dirp);
        i++;
    }
    return i;
}

// 
int handleseek(int of,int od,int e,char* search,char*targ_dir,char* curr_dir,char*prev_dir){
    char*abstarg_dir=malloc(MAX_PATH+1);
    rel2abs(abstarg_dir,targ_dir,curr_dir);
    int found;
    char matchpath[MAX_PATH+1];
    if(e){
        found=printmatching(matchpath,e,of,od,search,abstarg_dir,abstarg_dir);
        if(found==1){
            struct stat fstat;
            if(stat(matchpath,&fstat)){
                pcerror("seek: Getting stats of file");
                return -1;
            };
            if(S_ISDIR(fstat.st_mode)){
                if(fstat.st_mode&S_IXUSR){
                    chdir(matchpath);
                    strcpy(prev_dir,curr_dir);
                }
                else{
                    cmderror("seek",": Missing permissions for task!");
                }
            }
            else{
                if(fstat.st_mode&S_IRUSR){
                    FILE*fp=fopen(matchpath,"r");
                    if(fp==NULL){
                        perror("Opening matched file");
                        return -1;
                    }
                    char c=fgetc(fp);
                    while (c!=EOF)
                    {
                        printf("%c",c);
                        c=fgetc(fp);
                    }
                    printf("\n");
                }
                else{
                    cmderror("seek",": Missing permissions for task!");                    
                }
            }
        }
    }
    else{
        if(targ_dir){
            found=printmatching(matchpath,e,of,od,search,abstarg_dir,abstarg_dir);
        }
        else{
            found=printmatching(matchpath,e,of,od,search,curr_dir,curr_dir);
        }

        if(!found){
            printf("No match found!\n");
        }
    }
    free(abstarg_dir);
    return 0;
}

// Prints a list of all files/directories with exact name match for 'search', relative to the target directory
int seek(char*args,char* curr_dir,char*prev_dir){
    int res=0;
    char*argtoken=strtok(args," ");
    int of=0,od=0,e=0;
    char search[MAX_PATH+1]="\0";
    char targ_dir[MAX_PATH+1]="\0";
    
    while (argtoken){
        if(argtoken[0]=='-' ){
            if(strlen(argtoken)>1){
                // store flags
                for(int i=1;i<strlen(argtoken);i++){
                    if(argtoken[i]=='f'){
                        of=1;
                    }
                    else if(argtoken[i]=='d'){
                        od=1;
                    }
                    else if(argtoken[i]=='e'){
                        e=1;
                    }
                    else{
                        cmderror("seek",": Invalid flags!");
                        return -1;
                    }
                }
            }
            else{
                cmderror("seek",": Invalid flags!");
                return -1;
            }
        }
        else{
            if(strlen(search)&&strlen(targ_dir)){
                cmderror("seek",": Too many arguments!");
                return -1;
            }
            else if(strlen(search)){
                strcpy(targ_dir,argtoken);
            }
            else{
                strcpy(search,argtoken);
            }
        }
        
        argtoken=strtok(NULL," ");
    }
    if(strlen(search)==0){
        cmderror("seek",": 'search' can not be empty!");
        return -1;
    }
    if(of==1&&od==1){
        cmderror("seek",": Invalid flags!");
        return -1;
    }
    return handleseek(of,od,e,search,targ_dir,curr_dir,prev_dir);
}