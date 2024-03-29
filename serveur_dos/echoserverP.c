/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define MPROC 10
#define NB_PROC 5
pid_t Table_Fils[NB_PROC-1];

void echo(int connfd);


void handler(int sig){
	pid_t pid;
    //tant qu'il y a un fils zombie ont le termine
	while((pid=waitpid(-1,NULL,WNOHANG))>0){}
}

void handler_term(int sig){
    printf("\nterminaison propre :\n");
    for(int i=0;i< NB_PROC-1;i++){
        Kill(Table_Fils[i],SIGKILL);
    }
    printf("terminaison des fils terminer\n");
    printf("arrer du serveur UwU\n");
    exit(0);
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    Signal(SIGCHLD,handler);
    Signal(SIGINT,handler_term);
    pid_t child=1;
    char continu;
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(2121);

    for (int i=0 ; (i < (NB_PROC-1)) && (child!=0) ; i++){
        child = Fork();
        if(child!=0){
            Table_Fils[i]=child;
        }
    }
    

    while (1) {
        
        while((connfd = accept(listenfd, (SA *)&clientaddr, &clientlen))<0);
        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
        
        printf("server connected to %s (%s)\n", client_hostname,
            client_ip_string);

        while((Rio_readn(connfd,&continu,sizeof(char)))==1){
            echo(connfd);
        }
        
        Close(connfd);
    }
}

