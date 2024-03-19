/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define MPROC 10
#define NBRFILS 1

void echo(int connfd);


void handler(int sig){
	pid_t pid;
    //tant qu'il y a un fils zombie ont le termine
	while((pid=waitpid(-1,NULL,WNOHANG))>0){}
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    Signal(SIGCHLD,handler);
    pid_t child=1;
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    for (int i=0 ; (i < NBRFILS) && (child!=0) ; i++){
        child = Fork();
    }
    

    while (1) {
        
        while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen))<0){};
        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
        
        printf("server connected to %s (%s)\n", client_hostname,
            client_ip_string);

        echo(connfd);
        
        Close(connfd);
    }
}

