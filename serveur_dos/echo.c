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

    // lit le nom du fichier
    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    buf[strlen(buf)-1] = '\0';

    // ajoute le chemin du fichier
    char name[MAXLINE+11] = "./fichier/\0";
    strcat(name, buf);

    // ouvre le fichier
    fd = open(name, O_RDONLY, 0);
    if(fd < 0){
        tailleFichier = -1;
        Rio_writen(connfd, &tailleFichier, sizeof(off_t ));
    }
    else{

        // recupere la taille du fichier
        fstat(fd, &st);   
        tailleFichier = st.st_size;

        // envoie au client la taille du fichier
        Rio_writen(connfd, &tailleFichier, sizeof(off_t));

        // envoie au client la taille du paquet
        Rio_writen(connfd, &taillepaquet, sizeof(off_t));

        // recoit le byte de depart du fichier
        Rio_readn(connfd, &lu, sizeof(off_t));
        lseek(fd, lu, SEEK_SET);

        // envoie les paquets 1 a 1
        char doc[taillepaquet];
        while((lu + taillepaquet) < tailleFichier){
            lu += taillepaquet;
            Rio_readn(fd, doc, taillepaquet);
            Rio_writen(connfd, doc, taillepaquet);
        }

        // envoie le dernier paquet si existe paquet restant < taille d'un paquet
        Rio_readn(fd, doc, tailleFichier-lu);
        Rio_writen(connfd, doc, tailleFichier-lu);

        // ferme le fichier
        Close(fd);
    }

}

