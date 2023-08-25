#include "../headers.h"

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
    char og_dir[MAX_PATH+1];
    if(getcwd(og_dir,MAX_PATH+1)==NULL){
        pcerror("Getting current directory");
        return -1;
    }
    chdir(eff_path);
    char curr_dir[MAX_PATH+1];
    if(getcwd(curr_dir,MAX_PATH+1)==NULL){
        pcerror("Getting current directory");
        return -1;
    }

    DIR* dirp=opendir(curr_dir);
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
        if(getfilepath(filepath,curr_dir,entp->d_name)){
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
    
    res=printfiles(allfiles,i,l,a,curr_dir,digits(max_size));
    
    for(int i=0;i<MAX_FILES;i++){
        free(allfiles[i]);
    }
    free(allfiles);
    closedir(dirp);
    
    chdir(og_dir);
    
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