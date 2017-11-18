#include "dht.h"
#include <string.h>


/**
 * \fn void usage (char * arg)
 * \brief Affiche l'usage de la fonction sur la sortie d'erreur
 *
 * \param arg Nom du programme
 */
void usage(char * arg){
    fprintf(stderr, "usage: %s IP PORT\n", arg);
    exit(EXIT_FAILURE);
}



/**
 * \fn int main (int argc, char * argv[])
 * \brief Entrée du programme.
 *
 * \param argc Nombre de paramètres (+ nom du programme)
 * \param argv[] Tableau contenant les paramètres (+ nom du programme)
 * \return EXIT_SUCCESS arrêt normal, EXIT_FAILURE en cas d'erreur
 */
int main(int argc, char * argv[]){

    // initialisations des variables
    int port, sock, length, convert;
    struct sockaddr_in6 addr_server;
    //unsigned char ip[sizeof(struct in6_addr)];


    // vérification des arguments
    if(argc != 3){
        usage(argv[0]);
    }


    // vérification du port
    port = atoi(argv[2]);


    // convertir l'argument en adresse IPv6
    convert = inet_pton(AF_INET6, argv[1], &addr_server.sin6_addr); 
    if(convert <= 0){
        if(convert == 0)
            fprintf(stderr, "Erreur: l'adresse IP entrée est invalide\n");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }


    // initialisation socket
    PERRORMSG((sock=socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)), "socket");
   

    // initialisation adresse IP du serveur
    addr_server.sin6_family = AF_INET6;
    addr_server.sin6_port = port;
	length = sizeof(struct sockaddr_in);


    // on attache l'adresse IP du serveur au socket
    if(bind(sock, (struct sockaddr *)&addr_server, length) == ERROR){
        perror("bind");
        PERRORMSG(close(sock), "close");
        exit(EXIT_FAILURE);
    }


    // fermeture du socket
    PERRORMSG(close(sock), "close");

	// le programme s'est bien déroulé 
    return EXIT_SUCCESS;

}
