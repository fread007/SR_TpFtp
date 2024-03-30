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

    //li le nom du fichier
    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    buf[strlen(buf)-1] = '\0';

    //ajoute le chemain du fichier
    char name[MAXLINE+11] = "./fichier/\0";
    strcat(name,buf);

    //ouvre le fichier
    fd = open(name, O_RDONLY, 0);
    if(fd < 0){
        tailleFichier = -1;
        Rio_writen(connfd, &tailleFichier, sizeof(off_t ));
    }
    else{

        //recupere la taille du fichier
        fstat(fd, &st);   
        tailleFichier = st.st_size;

        //envois au client la taille du fichier
        Rio_writen(connfd, &tailleFichier, sizeof(off_t));

        //envois au client la taille du paquet
        Rio_writen(connfd, &taillepaquet, sizeof(off_t));

        //resoit le byte de depart du fichier
        Rio_readn(connfd, &lu, sizeof(off_t));
        lseek(fd, lu, SEEK_SET);

        //envois les paquet 1 par 1
        char doc[taillepaquet];
        while((lu + taillepaquet) < tailleFichier){
            lu += taillepaquet;
            Rio_readn(fd, doc, taillepaquet);
            Rio_writen(connfd, doc, taillepaquet);
        }

        //envois le dernier paquet (peut etre plus petit)
        Rio_readn(fd, doc, tailleFichier-lu);
        Rio_writen(connfd, doc, tailleFichier-lu);

        //ferme le fichier
        Close(fd);
    }

}

