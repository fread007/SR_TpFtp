/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

#define TAILLE_PAQUET 512

void echo(int connfd)
{
    char buf[MAXLINE];
    rio_t rio;
    int fd;
    struct stat st;
    off_t tailleFichier, lu, taillepaquet = TAILLE_PAQUET;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    buf[strlen(buf)-1] = '\0';

    char name[MAXLINE+11] = "./fichier/\0";
    strcat(name,buf);
    fd = open(name, O_RDONLY, 0);
    if(fd < 0){
        tailleFichier = -1;
        Rio_writen(connfd, &tailleFichier, sizeof(off_t ));
    }
    else{

        fstat(fd, &st);   
        tailleFichier = st.st_size;

        Rio_writen(connfd, &tailleFichier, sizeof(off_t));
        Rio_writen(connfd, &taillepaquet, sizeof(off_t));

        Rio_readn(connfd, &lu, sizeof(off_t));
        lseek(fd, lu, SEEK_SET);

        char doc[taillepaquet];
        while((lu + taillepaquet) < tailleFichier){
            lu += taillepaquet;
            Rio_readn(fd, doc, taillepaquet);
            Rio_writen(connfd, doc, taillepaquet);
        }
        Rio_readn(fd, doc, tailleFichier-lu);
        Rio_writen(connfd, doc, tailleFichier-lu);
        Close(fd);
    }

}

