#include "dht.h"
#include "fctsocket.h"
#include <pthread.h>



/**
 * \fn void usage (char * arg)
 * \brief Affiche l'usage de la fonction sur la sortie d'erreur
 *
 * \param arg Nom du programme
 */
void usage(char * arg){
    fprintf(stderr, "usage: %s IP PORT [-c IP PORT]\n", arg);
    fprintf(stderr, "For help use --help\n");
    fprintf(stderr, "For version use --version\n");
    exit(EXIT_FAILURE);
}





/**
 * \fn void parse_option (char * arg)
 * \brief Analyse les option --help et --version
 * permet de générer une page de man avec help2man
 *
 * \param arg L'option entrée par l'utilisateur
 */
void parse_option(char * arg[]){
    
    if(strcmp("--help", arg[1]) == 0){
        // option --help
        printf("Utilisation : %s IPv6 PORT [-c IP PORT]\n", arg[0]);
        printf("         ou : %s --help | --version\n", arg[0]);
        printf("Lance un serveur configuré comme suit:\n");
        printf("  - adresse IPv6: argument IPv6\n");
        printf("  - numéro de port: argument PORT\n");
        printf("Option -c permet de se connecter à un serveur:\n");
        printf(" - %s mon_IP mon_PORT -c IP_serveur PORT_serveur\n", arg[0]);
        exit(EXIT_SUCCESS);
    }

    if(strcmp("--version", arg[1]) == 0){
        // option --version
        printf("server 1.0\n");
        printf("Copyright 2017 Lionel Jung & David Lardenois\n");
        exit(EXIT_SUCCESS);
    }
    
    // option inconnue 
    fprintf(stderr, "Erreur: option inconnue\n");
    usage(arg[0]);
}



/*
 * \fn
 * \brief
 *
 * \param
 */
static void * keep_alive (void * server){
	// keep alive avec les server
	struct sockaddr_in6 * liste = server;
	while(1){
	
	}
	return NULL;
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
    int port, sock, type_mess, end = 0, test;
	int connexion = 0, nb_server = 0, sock_serv;
    socklen_t addrlen = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 liste_server[MAX_SERVER];
    struct sockaddr_in6 addr_server, addr_dest;
    char buf[MESS_MAX_SIZE], mess[MESS_MAX_SIZE], lg[3], type[2];
    char *hash = NULL, *ip_m = NULL, *get = NULL;
    DHT * t = NULL; 
	pthread_t ta;
    
    memset(mess, '\0', MESS_MAX_SIZE);
    memset(buf, '\0', MESS_MAX_SIZE);
    memset(lg, '\0', 3);
    memset(type, '\0', 2);

	// verification des arguments
	switch(argc){
		case 2:
			parse_option(argv);
			break;
		case 3:
			// cas normal, on continue le programme
			break;
		case 6:
			// cas avec demande de connexion (option -c)
			if(strcmp(argv[3], "-c") != 0){
				fprintf(stderr, "Erreur: %s option inconnue\n", argv[3]);
			}
			add_server(liste_server, argv[4], argv[5], &nb_server);
			connexion = 1;
			break;
		default:
			// erreur nombre d'arguments
			usage(argv[0]);
			break;
	}


    // vérification du port
    if((port = port_valide(argv[2])) == ERROR){
        fprintf(stderr, "Erreur: numero de port invalide\n");
        usage(argv[0]);
    }


    // convertir l'argument en adresse IPv6
    if(convert_ipv6(argv[1], argv[2], &addr_server) == ERROR){
        usage(argv[0]);
    } 


    // initialisation socket client
    sock = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);


    // initialisation adresse IP du serveur
    addr_server.sin6_family = AF_INET6;
    addr_server.sin6_port = htons(port);


    // on attache l'adresse IP du serveur au socket
    lier_socket6(sock, &addr_server);


    // on dit aux autres serveurs qu'on est là
   	if(connexion == 1){
		// init socket serveur
		sock_serv = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		lier_socket6(sock_serv, &addr_server);

		// lancement d'un thread qui s'occupe du keep alive
   		pthread_create(&ta, NULL, keep_alive, &liste_server);
	}


    // communications du serveur
    while(end != 1){
        
        if(recvfrom(sock, buf, MESS_MAX_SIZE, 0,
            (struct sockaddr *)&addr_dest, &addrlen) == ERROR){
            perror("recvfrom");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // analyse du message
        type_mess = get_type_from_mess(buf);
        hash = extraire_hash_mess(buf);

        // on détermine ce qu'on doit faire
        switch(type_mess){
    
            case PUT:
                 ip_m = extraire_ip_mess(buf);
                // message de type PUT
                if((test = put_hash(hash, ip_m, &t)) == ERROR){
                    fprintf(stderr, "put_hash failed\n");
                }
                // on doit envoyer le hash aux autres serveurs !
                if(test != NTD){
                    printf("New Entry in table: IP %s has hash %s\n",ip_m,hash);
                    free(ip_m);
                    free(hash);
                }
                break;

            case GET:
                // message de type GET
                get = get_hash(hash, t);
                if(get == NULL){
                    get = malloc(25);
                    memcpy(get, "no IP match with request\0", 25);
                }
                printf("GET: %s\n", get);
                // on doit envoyer un message au client
                // creation du message
                remplir_lg("", get, lg);
                remplir_type(HAVE, type);
                creation_chaine(type, lg, get, mess);

                envoyer_mess6(sock, mess, addr_dest);
                free(get);
                free(hash);
                break;
          
            case NEW:
                // un nouveau serveur nous notifie
                break;

            case DECO:
                // un serveur se déconnecte
                
                break;

            case HAVE:
                // un serveur nous informe de ses modification
                ip_m = extraire_ip_mess(buf);
                printf("Reception d'une nouvelle entree\n");
                if((test = put_hash(hash, ip_m, &t)) == ERROR){
                    fprintf(stderr, "put_hash failed\n");
                }
                printf("New Entry in table: IP %s has hash %s\n", ip_m, hash);
                free(ip_m);
                free(hash);
                break;

            case EXIT:
                // on demande au serveur de s'arreter
                // on vérifie le code d'acces
                if(check_access_code(hash) == 0) end = 1;
                else end = 0;
                break;

            default:
                // type de message inconnu
                fprintf(stderr,"Erreur: message type %d inconnu\n",type_mess);
                break;
        } // fin switch
   
        // remise à zéro
        memset(mess, '\0', MESS_MAX_SIZE);
        memset(buf, '\0', MESS_MAX_SIZE);

    } // fin boucle 

    printf("Arret du serveur %s\n", argv[1]);
   
	// arret du keep alive
	pthread_exit(&ta);

    // il faut notifier les autres serveurs qu'on s'arrete

    // fermeture du socket
    fermer_socket(sock);

    // suppression de la table
    supp_dht(t);

    // le programme s'est bien déroulé 
    exit(EXIT_SUCCESS);

}
