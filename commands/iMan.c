#include "../headers.h"

/*  iMan    */

int iMan(char* cmd){
    if(cmd==NULL||strlen(cmd)==0){
        cmderror("iMan :","missing arguments");
        return -1;
    }
    struct addrinfo* hints=(struct addrinfo*)malloc(sizeof(struct addrinfo)), *res;
    /*
    struct addrinfo {
        int              ai_flags;
        int              ai_family;
        int              ai_socktype;
        int              ai_protocol;
        socklen_t        ai_addrlen;
        struct sockaddr *ai_addr;
        char            *ai_canonname;
        struct addrinfo *ai_next;
    };
    */

    if(getaddrinfo(MANHNAME,"http",NULL,&res)){
        pcerror("Getting address info");
        return -1;
    }

    int socketfd=socket(AF_INET,res->ai_socktype,res->ai_protocol);
    if(socketfd < 0){
        pcerror("Unable to create socket\n");
        return -1;
    }
    printf("SOCKTYPE: %d\n",res->ai_socktype);
    
    if(connect(socketfd,res->ai_addr,res->ai_addrlen)){
        pcerror("Unable to connect to the server");
        close(socketfd);
        return -1;
    }

    // Request
    char request[100]="\0";
    strcpy(request,"GET ");
    strcat(request,"/");
    // ?topic=CMD&section=all
    strcat(request,"?topic=");
    strcat(request,strtok(cmd," "));
    strcat(request,"&section=all");
    strcat(request," HTTP/1.1\r\nHost: ");
    strcat(request,MANHNAME);
    strcat(request,"\r\n\r\n");

    // printf("%s\n\n",request);

    if(send(socketfd,request,strlen(request),0)<strlen(request)){
		pcerror("Sending request");
        close(socketfd);
        return -1;
	}
    char response[MAX_MANRESP+1];
    ssize_t numres= recv(socketfd,response,MAX_MANRESP,0);
    if(strstr(response,"No matches for ")){
        cmderror("'iMan': ","No such command");
        close(socketfd);
        return -1;
    }
    char*start=strstr(response,"NAME\n");
    if(start==NULL){
        pcerror("No such command");
        close(socketfd);
        return -1;
    }
    printf("%s",start);
    while(recv(socketfd,response,MAX_MANRESP,0)){
        char* end=strstr(response,
        "</PRE>"
        // "reutils"
        );
        if(end){
            response[strlen(response)-strlen(end)]='\0';
        }
        printf("%s",response);
    }
    printf("\n");
    
    close(socketfd);
    return 0;
}