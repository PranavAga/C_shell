/*
COMMANDS IMPLEMENTED HERE:
warp, peek, pastevents, proclore
*/
#include "headers.h"

/*  warp    */

void tilde2abs(char*arg,char*launch_dir,char*newdir){
    strcpy(newdir,launch_dir);
    size_t i=1;
    size_t ldir_size=strlen(launch_dir);
    while(i<strlen(arg)){
        newdir[ldir_size+i-1]=arg[i];
        i++;
    }
    newdir[ldir_size+i-1]='\0';
}

int hyph2abs(char* cmd,char*arg,char*prev_dir,char*newdir){
    if(strlen(arg)>1){
        cmderror(cmd,": '-' used wrong");
        return -1;
    }
    if(strlen(prev_dir)==0){
        cmderror(cmd,": no previous directory");
        return -1;
    }
    strcpy(newdir,prev_dir);
    return 0;
}

int handlewrap(char*arg,char*launch_dir,char*prev_dir){
    char newdir[MAX_PATH+1];
    if (arg[0]=='~'){
        tilde2abs(arg,launch_dir,newdir);
    }
    else if(arg[0]=='-'){
        if(hyph2abs(WARP,arg,prev_dir,newdir)){
            return -1;
        }
    }
    else{
        if(chdir(arg)){
            pcerror("warping");
            return -1;
        }
        return 0;
    }
    if(chdir(newdir)){
        pcerror("warping (~/-)");
        printf("%s\n",newdir);
        return -1;
    }
    return 0;
}

// changes the directory that the shell is currently in
int warp(char* args,char* launch_dir,char*prev_dir){
    if(strlen(args)==0){
        cmderror("warp","need arguments");
        return -1;
    }
    char curr_dir[MAX_PATH+1];
    int res=0;
    char*argtoken=strtok(args," ");
    while (argtoken)
    {
        if(getcwd(curr_dir,MAX_PATH+1)==NULL){
            pcerror("Getting current directory");
            return -1;
        }
        res=handlewrap(argtoken,launch_dir,prev_dir);
        if(res){
            return res;
        }
        strcpy(prev_dir,curr_dir);
        argtoken=strtok(NULL," ");
    }
    return res;
}

/*  peek    */

int handleflagspeek(char*flag,int*l,int*a){
    size_t len=strlen(flag);
    if(len>3){
        cmderror(PEEK,": invalid flags");
        return -1;
    }
    else {
        for(int i=1;i<len;i++){
            if(flag[i]=='l'){
                *l=1;
            }
            else if(flag[i]=='a'){
                *a=1;
            }
            else{
                cmderror(PEEK,": invalid flags character");
                return -1;
            }
        }
    }
    return 0;
}

void reorder(char**files,int size){
    char temp[MAX_PATH];
    for(int i=1;i<size;i++){
        for(int j=0;j<size-i;j++){
            if(strcmp(files[j],files[j+1])>0){
                strcpy(temp,files[j]);
                strcpy(files[j],files[j+1]);
                strcpy(files[j+1],temp);
            }
        }
    }
}

void permsprint(mode_t mode){
    printf("%s%s%s%s%s%s%s%s%s%s "
    ,S_ISDIR(mode)?D:NO
    ,mode&S_IRUSR?R:NO,mode&S_IWUSR?W:NO,mode&S_IXUSR?X:NO
    ,mode&S_IRGRP?R:NO,mode&S_IWGRP?W:NO,mode&S_IXGRP?X:NO
    ,mode&S_IROTH?R:NO,mode&S_IWOTH?W:NO,mode&S_IXOTH?X:NO
    );
    return;
}

unsigned long digits(file_size n)  
{  
    unsigned long counter=0;  
    while(n){  
        n=n/10;  
        counter++;  
    }  
    return counter;  
}  

int getfilepath(char*filepath,char*dir_path,char*file){
    if(dir_path[0]=='.'){
            if(strlen(dir_path)==1||strcmp(dir_path,"./")==0){
                strcpy(filepath,"./");
            }
            else{
                if(dir_path[1]=='/'&&strlen(dir_path)>2){                    
                    strcpy(filepath,dir_path);
                    strcat(filepath,"/");
                }
                else{
                    cmderror("peek",": directory path is invalid");
                    return -1;
                }
            }
        }
    else if (dir_path[0]!='/'){// dir inside cwd
        strcpy(filepath,"./");
        strcat(filepath,dir_path);
        strcat(filepath,"/");
    }
    else{
        strcpy(filepath,dir_path);
        strcat(filepath,"/");
    }
    strcat(filepath,file);
    return 0;
}

int printfiles(char**files,int size,int l,int a,char* dir_path,int max_size_digits){
    char mno2threelet[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    for(int i=0;i<size;i++){
        if(!a&&files[i][0]=='.'){
            continue;
        }
        char filepath[MAX_PATH+1];
        struct stat fstat;
        if(getfilepath(filepath,dir_path,files[i])){
            return -1;
        }

        if(stat(filepath,&fstat)){
            pcerror("Getting stats of file in peek");
            return -1;
        };
        if(l){
            struct passwd* pf_user=getpwuid(fstat.st_uid);
            if(pf_user==NULL){
                pcerror("Getting password file of creator");
            }
            struct passwd* pf_grp=getpwuid(fstat.st_gid);
            if(pf_grp==NULL){
                pcerror("Getting password file of creator");
            }
            // <permissions> 
            permsprint(fstat.st_mode);

            // <nhardlinks>   <owner>           <group>                   <size>
            printf("%ld "    "%s\t"            "%s\t"                  "%*ld\t",
            fstat.st_nlink,pf_user->pw_name,pf_grp->pw_name,max_size_digits,fstat.st_size);
            
            time_t curr_time;
            time(&curr_time);
            struct tm* mtime=localtime(&(fstat.st_mtime));
            //                 <Mon     Date
            printf(             "%s "    "%2d ",
            mno2threelet[mtime->tm_mon],mtime->tm_mday);
            if(difftime(curr_time,fstat.st_mtime)>MIN_TIMEDIFF){
                //      Year>
                printf("%d\t",mtime->tm_year+1900);
            }
            else{
                //       hrs:            min>
                printf("%02d:"           "%02d\t",
                        mtime->tm_hour,mtime->tm_min);
            }
        }
        if(S_ISDIR(fstat.st_mode)){
            printf(BLUE"%s"DEFAULT"\n",files[i]);        
        }
        else if(fstat.st_mode&S_IXUSR){
            printf(GREEN"%s"DEFAULT"\n",files[i]);        
        }
        else{
            printf(WHITE"%s"DEFAULT"\n",files[i]);
        }
    }
    return 0;
}

int handlepeek(char*path,int l,int a,char*launch_dir,char*prev_dir){
    int res=0;
    char eff_path[MAX_PATH+1];
    char** allfiles=malloc(sizeof(char*)*MAX_FILES);
    for(int i=0;i<MAX_FILES;i++){
        allfiles[i]=malloc(MAX_PATH+1);
    }
    if(path==NULL){
        strcpy(eff_path,".");
    }
    else{
        if (path[0]=='~'){
            tilde2abs(path,launch_dir,eff_path);
        }
        else if(path[0]=='-'){
            if(hyph2abs(PEEK,path,prev_dir,eff_path)){
                return -1;
            }
        }
        else{
            strcpy(eff_path,path);
        }
    }
    
    DIR* dirp=opendir(eff_path);
    if(dirp==NULL){
        pcerror("peek: Opening directory");
        return -1;
    }
    int i=0;
    struct dirent*entp=readdir(dirp);
    file_size max_size=0;
    while (entp)
    {
        struct stat fstat;
        char filepath[MAX_PATH+1];
        if(getfilepath(filepath,eff_path,entp->d_name)){
            return-1;
        }
        if(stat(filepath,&fstat)){
            pcerror("peek: Getting stats of file");
            return -1;
        };
        if(max_size<fstat.st_size){
            max_size=fstat.st_size;
        }
        strcpy(allfiles[i],entp->d_name);
        entp=readdir(dirp);
        i++;
    }
    reorder(allfiles,i);
    
    res=printfiles(allfiles,i,l,a,eff_path,digits(max_size));
    
    for(int i=0;i<MAX_FILES;i++){
        free(allfiles[i]);
    }
    free(allfiles);
    closedir(dirp);
    return res;
}

// lists all files and directories in specified or current directory
int peek(char*args,char* launch_dir,char*prev_dir){
    int res=0;
    char*argtoken=strtok(args," ");
    char*flag1=NULL;
    char*flag2=NULL;
    char*path=NULL;
    while (argtoken){
        if(argtoken[0]=='-' && strlen(argtoken)>1){
            if(flag1){
                flag2=argtoken;
            }
            else{
                flag1=argtoken;
            }
        }
        else{
            path=argtoken;
        }
        
        argtoken=strtok(NULL," ");
    }
    int a=0,l=0;
    if(flag1){
        res=handleflagspeek(flag1,&l,&a);
    }
    if(flag2){
        res=handleflagspeek(flag2,&l,&a);
    }
    if(res){
        return res;
    }
    return handlepeek(path,l,a,launch_dir,prev_dir);
}

/*  pastevents  */

// Manage 'pastevents' command
char*pastevents(char*launch_dir,char*cmd){
    char new_cmd[MAX_INP]={ 0 };
    char filepath[MAX_PATH+1]={0};
    strncpy(filepath,launch_dir,strlen(launch_dir));
    strcat(filepath,"/.");
    strcat(filepath,PASTEVENTS);

    if(strcmp(cmd,PASTEVENTS)==0){
        FILE*history=fopen(filepath,"r");
        if(history==NULL){
            perror("Opening 'pastevents' file");
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
    return strlen(new_cmd)>0?new_cmd:NULL;
}

/*  proclore */

void pid2str(char* pidstr,pid_t pid){
    pid_t n = pid;
    int len=0;
    while (n)
    {
        n /= 10;
        len++;
    }
    int dig;
    n = pid;
    for (int i = 0; i < len; i++){
        dig = n % 10;
        n/=10;
        pidstr[len-(i+1)]=dig+'0';
    }
    pidstr[len] = '\0';
}

int proclore(char*args,pid_t shellp,char*launch_dir){
    /**info for a proc: /proc/<pid>
     * pid : /stat(1)
     * process status : /stat(3) (8)==(5)
     * Process Group : /stat(5)
     * Virtual memory : /stat(23)
     * executable path : /exe
    */
    char pidstr[10];

    char*newpid=strtok(args," ");
    if(newpid==NULL|| strlen(newpid)==0){// shell's proc
        pid2str(pidstr,shellp);
        }
    else{
        strcpy(pidstr,newpid);
    }

    char status[2];
    int pgrp;
    int tpgid;
    unsigned long vsize;
    char exepath[MAX_PATH+1]="\0";

    int _1;
    char _2[17];
    int _4,_6,_7,_9,_10,_11,_12,_13,_14;
    long _16,_17,_18,_19,_20,_21,_15;
    unsigned long long _22;

    
    char procpid[6+strlen(pidstr)+1];
    strcpy(procpid,"/proc/");
    strcat(procpid,pidstr);

    char procstat[strlen(procpid)+7];
    strcpy(procstat,procpid);
    strcat(procstat,"/stat");

    char procexec[strlen(procpid)+7];
    strcpy(procexec,procpid);
    strcat(procexec,"/exe");

    FILE*statp=fopen(procstat,"r");
    if(statp==NULL){
        pcerror("Couldn't open process's stat file");
        return -1;
    }
    fscanf(statp,"%d %s %s %d %d %d %d %d %d %d %d %d %d %d %ld %ld %ld %ld %ld %ld %llu %lu",
    &_1,_2,status,&_4,&pgrp,&_6,&_7,&tpgid,&_9,&_10,&_11,&_12,&_13,&_14,&_15,&_16,&_17,&_19,&_20,&_21,&_22,&vsize);
    
    fclose(statp);

    printf("pid : %d\n",_1);
    printf("process status : %s",status);
    if(pgrp==tpgid){// foreground
        printf("+\n");
    }
    else{
        printf("\n");
    }
    printf("Process Group : %d\n",pgrp);
    printf("Virtual memory : %lu\n",vsize);

    if(readlink(procexec,exepath,MAX_PATH)<-1){
        pcerror("Couldn't find executable path");
        return -1;
    }
    if(strstr(exepath,launch_dir)){
        char*relfile=exepath+strlen(launch_dir)+1;
        strcpy(exepath,"~/");
        strcat(exepath,relfile);
    }
    printf("executable path : %s\n",exepath);
    strcpy(exepath,"");
    return 0;
}