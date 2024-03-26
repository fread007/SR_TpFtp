/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

void echo(int connfd)
{
    char buf[MAXLINE];
    rio_t rio;
    int fd;
    struct stat st;
    off_t tailleFichier;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    buf[strlen(buf)-1]='\0';

    char name[MAXLINE+11]="./fichier/\0";
    strcat(name,buf);

    fd=open(name,O_RDONLY,0);
    if(fd<0){
        tailleFichier = -1;
        Rio_writen(connfd, &tailleFichier, sizeof(off_t ));
    }
    else{

        fstat(fd,&st);   
        tailleFichier=st.st_size;

        Rio_writen(connfd, &tailleFichier, sizeof(off_t ));

        char doc[tailleFichier];
        Rio_readn(fd,doc,tailleFichier);
        Rio_writen(connfd,doc,tailleFichier);
        Close(fd);
    }

}

