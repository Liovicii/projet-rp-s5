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
	//char str[INET6_ADDRSTRLEN];

    // vérification des arguments
    if(argc != 3){
        usage(argv[0]);
    }

    // vérification du port
    port = atoi(argv[2]);
	if( (port > 65536) || (port <= 0) ){
		fprintf(stderr, "Erreur: numero de port invalide\n");
		usage(argv[0]);
	}

    // convertir l'argument en adresse IPv6
    convert = inet_pton(AF_INET6,argv[1],(void*)&addr_server.sin6_addr.s6_addr);
    if(convert <= 0){
        if(convert == 0){
            fprintf(stderr, "Erreur: l'adresse IP entrée est invalide\n");
			usage(argv[0]);
	    }
		else{
            perror("inet_pton");
		}
    }


    // initialisation socket
    PERRORMSG((sock=socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)), "socket"); 

    // initialisation adresse IP du serveur
    addr_server.sin6_family = AF_INET6;
    addr_server.sin6_port = port;
    length = sizeof(struct sockaddr_in6);
/*
	// affichages tests
	printf("port: %d\n", addr_server.sin6_port);
	printf("IPv6: %s\n", 
	inet_ntop(AF_INET6,&addr_server.sin6_addr,str,INET6_ADDRSTRLEN));
*/
    // on attache l'adresse IP du serveur au socket
    if(bind(sock, (struct sockaddr *)&addr_server, length) == ERROR){
        perror("bind");
		printf("errno: %d\n", errno);
        PERRORMSG(close(sock), "close");
        exit(EXIT_FAILURE);
    }


    // fermeture du socket
    PERRORMSG(close(sock), "close");

    // le programme s'est bien déroulé 
    return EXIT_SUCCESS;

}
