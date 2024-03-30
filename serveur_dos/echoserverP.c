/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define MPROC 10
#define NB_PROC 5
pid_t Table_Fils[NB_PROC-1];

void echo(int connfd);

// handler pour signal SIGCHLD
void handler(int sig){
	pid_t pid;
    // tant qu'il y a un fils zombie on le termine
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0){}
}

// handler pour signal SIGINT <ctr+c>
void handler_term(int sig){
    printf("\nterminaison propre :\n");
    // terminaison des fils
    for(int i = 0; i < NB_PROC-1; i++){
        printf("Fils %d : %d \n", i, Table_Fils[i]);
        kill(Table_Fils[i], SIGKILL);
    }
    fprintf(stderr, "terminaison des fils terminee\n");
    fprintf(stderr, "arret du serveur UwU\n");
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
    Signal(SIGCHLD, handler);
    pid_t child = 1;
    char continu;
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(2121);

    // creation et recensement des fils
    for (int i=0 ; (i < (NB_PROC-1)); i++){
        child = Fork();
        if(child != 0){
            Table_Fils[i] = child;
        }
        else{
            break;
        }
    }

    // redefinition du signal SIGINT du pere
    if(child != 0){
        Signal(SIGINT, handler_term);
    }
    

    while (1) {
        // tous les processus attendent une connection  
        while((connfd = accept(listenfd, (SA *)&clientaddr, &clientlen)) < 0);
        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
        
        printf("serveur connecté à %s (%s)\n", client_hostname,
            client_ip_string);

        // teste si le client veut continuer
        while((Rio_readn(connfd, &continu, sizeof(char))) == 1){
            echo(connfd);
        }
        
        // fermeture de la connection
        Close(connfd);
    }
}

