/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd,resultatfd;
    char buf[MAXLINE];
    off_t tailleFichier;
    //rio_t rio;


    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd("im2ag-202-16", 2121);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");
    printf("ftp> ");
    
    Fgets(buf, MAXLINE, stdin);
    Rio_writen(clientfd, buf, strlen(buf));

    Rio_readn(clientfd,&tailleFichier,sizeof(off_t));
    if(tailleFichier==-1){
        fprintf(stderr,"erreur lors de l'ouverture du fichier\n");
    }
    else{

        char doc[tailleFichier];
        time_t debut = time(NULL);
        Rio_readn(clientfd,doc,tailleFichier);
        time_t fin = time(NULL);
        buf[strlen(buf)-1]='\0';
        char name[MAXLINE+11]="./fichier/\0";
        strcat(name,buf);
        resultatfd=open(name,O_WRONLY|O_CREAT|O_TRUNC,0644);
        write(resultatfd,doc,tailleFichier);
        printf("transfer reussit\n");
        printf("%ld bites resus en %ld seconde ",tailleFichier,(fin-debut));
        if ((fin-debut)!=0){
            printf("(%ld kbit/s)\n",(tailleFichier/(fin-debut)));   
        }
        else{
            printf("(calcule imposible temps trop cour)\n");
        }
    }
    
    Close(clientfd);

    exit(0);
}
