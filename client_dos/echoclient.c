/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd, resultatfd;
    char init[MAXLINE];
    off_t tailleFichier, taillePaquet;
    char *buf = init;
    char continu;
    struct stat st;
    off_t lu, dejaLu;

    if(argc!=2){
        printf("adresse serveur manquente. <<./echoclient IP>>\n");
        exit(0);
    }


    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(argv[1], 2121);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");
    // boucle de requetes du client
    while(1){
        // debut de la boucle
        printf("ftp> ");

        // recupere une ligne au clavier
        // represente la commande a executer
        // commandes implementees : get ; bye
        Fgets(buf, MAXLINE, stdin);

        // teste si l'entree est la commande "bye"
        if(strcmp(buf, "bye\n") == 0){
            continu = 0;
            Rio_writen(clientfd, &continu, sizeof(char));
            // dans ce cas on arrete le processus
            Close(clientfd);
            exit(0);
        }
        continu = 1;
        Rio_writen(clientfd, &continu, sizeof(char));

        // nous avons 2 commandes de 3 caracteres , le debut du fichier est au 5eme caractere
        buf+=4;

        // envoie le nom du fichier au serveur ( a noter ici que le nom ce fini par \n)
        Rio_writen(clientfd, buf, strlen(buf));

        // on recupere la taille du fichier qu'on va recevoir
        Rio_readn(clientfd, &tailleFichier, sizeof(off_t));

        // si la taille est -1 alors on a eu un probleme
        if(tailleFichier == -1){
            fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        }
        else{

            // on recupere la taille d'un paquet
            Rio_readn(clientfd, &taillePaquet,sizeof(off_t));

            // vu que l'on a un nom de fichier finissant par \n on remplace le dernier caractere par \0
            buf[strlen(buf)-1] = '\0';

            // on rajoute le chemain d'acces des fichiers
            char name[MAXLINE+11] = "./fichier/\0";
            strcat(name, buf);

            int exist = stat(name, &st);
            if(exist != -1){
                lu = st.st_size;
                dejaLu = st.st_size;
            }
            else{
                // variable qui nous permet de suivre l'evolution de l'ecriture
                lu = 0;
                dejaLu = 0;
            }
            Rio_writen(clientfd, &lu, sizeof(off_t));

            // on ouvre le fichier en ecriture
            resultatfd = open(name, O_WRONLY | O_CREAT | O_APPEND, 0644);

            // si on  obtient un descripteur non valide on s'arrete
            if(resultatfd < 0){
                fprintf(stderr,"Erreur d'ouverture-ecriture\n");
                exit(1);
            }

            lseek(resultatfd, lu, SEEK_SET);

            // on commence a initialiser les variables utiles a la recuperation du fichier
            // tableau pour chaque paquet
            char doc[taillePaquet]; // mauvaise reception de la taille
            // chronometre pour le temps du transfert
            time_t debut = time(NULL);

            // tant qu'on n'a pas recus tout le fichier
            while((lu + taillePaquet) < tailleFichier){
                // on ajoute le decalage
                lu += taillePaquet;
                // on recupere un paquet
                Rio_readn(clientfd, doc, taillePaquet);
                // on l'ecrit 
                write(resultatfd, doc, taillePaquet);
            }
            
            // on recupere les dernieres donnees < un paquet entier
            Rio_readn(clientfd, doc, (tailleFichier-lu));
            // on ecrit les dernieres donnees
            write(resultatfd, doc, (tailleFichier-lu));

            lu += (tailleFichier-lu);

            // on ferme le fichier d'ecriture
            Close(resultatfd);

            // fin du timer
            time_t fin = time(NULL);

            // on teste si le fichier est complet
            if(lu == tailleFichier){
                printf("transfert reussit\n");
                float taille = (float)(tailleFichier-dejaLu)/1000;
                
                if ((fin-debut) != 0){
                    taille /=1000;
                    float res = taille / (fin-debut);
                    printf("%.3f mega octets recus en %ld secondes (%.2f Mbit/s)\n", taille, (fin-debut), res);
                }
                else{
                    printf("%.3f kilo octets recus en %ld secondes ",taille,(fin-debut));
                    printf("(calcul impossible pour un transfert aussi rapide)\n");
                }
            }
        }
        buf = init;
    }
}
