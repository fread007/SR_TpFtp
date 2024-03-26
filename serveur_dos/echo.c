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

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    buf[strlen(buf)-1]='\0';
    
    fd=open(buf,O_RDONLY,0);
    fstat(fd,&st);   

    Rio_writen(connfd, &st.st_size, sizeof(off_t ));

    char doc[st.st_size];
    Rio_readn(fd,doc,st.st_size);
    Rio_writen(connfd,doc,st.st_size);

    Close(fd);


}

