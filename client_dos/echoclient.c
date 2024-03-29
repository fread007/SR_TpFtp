/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd,resultatfd;
    char toto[MAXLINE];
    off_t tailleFichier,taillepacket;
    char *buf=toto;
    char continu;
    struct stat st;
    off_t lu;


    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd("im2ag-202-12", 2121);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");
    //boucle de demande du clien
    while(1){
        //debut de la boucle
        printf("ftp> ");

        //recupere une ligne au clavier
        //represente la commande a executer
        //commande implementer : get ; bye
        Fgets(buf, MAXLINE, stdin);

        //test si on a entree "bye"
        if(strcmp(buf,"bye\n")==0){
            continu=0;
            Rio_writen(clientfd,&continu,sizeof(char));
            //dans ce cas ont arrete le processus
            Close(clientfd);
            exit(0);
        }
        continu=1;
        Rio_writen(clientfd,&continu,sizeof(char));

        //nous avon 2 commande de 3 caractere , le debt du fichier est au 5eme caractere
        buf+=4;

        //envois le nom du fichier au serveur ( a noter ici le nom ce fini par \n)
        Rio_writen(clientfd, buf, strlen(buf));

        //ont recupere la taille du fichier qu'on va resevoir
        Rio_readn(clientfd,&tailleFichier,sizeof(off_t));

        //si la taille est -1 alors ont a eu un probleme
        if(tailleFichier==-1){
            fprintf(stderr,"erreur lors de l'ouverture du fichier\n");
        }
        else{

            //ont recupere la taille d'un packet
            Rio_readn(clientfd,&taillepacket,sizeof(off_t));

            //vue que l'n a un nom de fichier fnissant par \n ont le remplace par \0
            buf[strlen(buf)-1]='\0';

            //ont rajoute le chemain d'acce des fichier
            char name[MAXLINE+11]="./fichier/\0";
            strcat(name,buf);

            int exist=stat(name,&st);
            if(exist!=-1){
                lu=st.st_size;
            }
            else{
                //varioable qui nous permer de suivre l'evolution de l'ecriture
                lu=0;
            }
            Rio_writen(clientfd,&lu,sizeof(off_t));

            //ont ouvre le fichier en ecriture
            resultatfd=open(name,O_WRONLY|O_CREAT | O_APPEND,0644);

            //si ont obtien un descripteur non valide ont s'arrete
            if(resultatfd<0){
                fprintf(stderr,"erreur ouverture ecriture\n");
                exit(1);
            }

            lseek(resultatfd,lu,SEEK_SET);

            //ont commence a initialiser les variable utile a la recuperation du fichier
            //tableau pour chaque packet
            char doc[taillepacket]; //movaise reseption de la taille
            //cronometre le temps
            time_t debut = time(NULL);
            


            //tant qu'on a aps ressus tout le fichier
            while((lu+taillepacket)<tailleFichier){
                fprintf(stderr,"ouiiii\n");
                //ont ajoute le decalage
                lu+=taillepacket;
                //ont recupere u packet
                Rio_readn(clientfd,doc,taillepacket);
                //on l'ecrit 
                write(resultatfd,doc,taillepacket);
            }
            
            //ont recupere les derneir donner
            Rio_readn(clientfd,doc,(tailleFichier-lu));
            //ont ecris les dernier donner
            write(resultatfd,doc,(tailleFichier-lu));

            lu+=(tailleFichier-lu);

            //on ferme le fichier d'ecriture
            Close(resultatfd);

            //fin du timer
            time_t fin = time(NULL);

            //ont teste si le fichier est compler
            if(lu==tailleFichier){
                printf("transfer reussit\n");
                printf("%ld bites resus en %ld seconde ",tailleFichier,(fin-debut));
                if ((fin-debut)!=0){
                    printf("(%ld kbit/s)\n",(tailleFichier/(fin-debut)));   
                }
                else{
                    printf("(calcule imposible temps trop cour)\n");
                }
            }
        }
        buf=toto;
    }
}
