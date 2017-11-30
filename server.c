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



/* \fn
 * \brief
 *
 * \param
 *//*
static void * recv_server (void * infos){
    if(infos == NULL){
        return NULL;
    }
    struct ka_data * args = infos;    
    struct sockaddr_in6 addr_dest;
    socklen_t addrlen = sizeof(struct sockaddr_in6);
    char buf[MESS_MAX_SIZE];

    // attente d'un message du serveur    
    if(recvfrom(args->sockfd, buf, MESS_MAX_SIZE, 0,
        (struct sockaddr *)&addr_dest, &addrlen) == ERROR){
        perror("recvfrom");
           close(args->sockfd);
        exit(EXIT_FAILURE);
    }


    return NULL;
}

*/




/*
 * \fn static void * keep_alive (void * server)
 * \brief Fonction appelé par un thread qui s'occupe du keep
 *     alive entre les serveurs.
 *
 * \param server Une structure contenant les données nécessaires
 */
/*
static void * keep_alive (void * server){
    // keep alive avec les server
    int i;
    socklen_t addrlen = 2;
    struct sockaddr_in6 addr_dest;
    struct ka_data * args;
    char buf[2]; 
    memset(buf, '\0', 2);

    if(server != NULL) args = server;
    else return NULL;

    while(1){
        for(i = 0; i < args->nb_serv; i++){
            remplir_type(KEEP_ALIVE, buf);
            creation_chaine(buf, "", "", "");
            // on demande si le serveur est en vie
            envoyer_mess6(args->sockfd, buf, args->liste[i]);
            memset(buf, '\0', 2);
            // on attend un réponse
            if(recvfrom(args->sockfd, buf, MESS_MAX_SIZE, 0,
                (struct sockaddr *)&addr_dest, &addrlen) == ERROR){
                perror("recvfrom");
                close(args->sockfd);
                exit(EXIT_FAILURE);
            }
            memset(buf, '\0', 2);

        } // fin for
    } // fin while
    return NULL;
}

*/


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
    int port;
	int type_mess;
	int end = 0;
	int test;
    int connexion = 0, nb_server = 0;
	//int sock_serv;

	int sock[4];
	struct sockaddr_in6 envoi_reception[4];
	struct timeval waitTh;	
	int max_sd;
	fd_set read_sds;
	int ret;

    socklen_t addrlen = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 liste_server[MAX_SERVER];
    //struct sockaddr_in6 addr_server, addr_server_threads;
    //struct sockaddr_in6 addr_dest;
    
    char buf[MESS_MAX_SIZE], mess[MESS_MAX_SIZE], lg[3], type[2];
    char *hash = NULL;
	char *ip_m = NULL;
	char *get = NULL;
  
    DHT * t = NULL; 
    //pthread_t recv_server_thread;
    //pthread_t send_server_thread;
    //pthread_t keep_alive_thread;
   // struct ka_data thread_arg;
    
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
    if(convert_ipv6(argv[1], argv[2], &envoi_reception[0]) == ERROR){
        usage(argv[0]);
    } 

    if(convert_ipv6(argv[1], argv[2], &envoi_reception[1]) == ERROR){
        usage(argv[0]);
    } 

    if(convert_ipv6(argv[1], argv[2], &envoi_reception[2]) == ERROR){
        usage(argv[0]);
    } 

    if(convert_ipv6(argv[1], argv[2], &envoi_reception[3]) == ERROR){
        usage(argv[0]);
    } 


    // initialisation socket client
    sock[0] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[1] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[2] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[3] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    // initialisation adresse IP du serveur
	// Reception client    
	envoi_reception[0].sin6_family = AF_INET6;
    envoi_reception[0].sin6_port = htons(port);
	//setip6(argv[1],&envoi_reception[3]);

	// Envoi client
    envoi_reception[1].sin6_family = AF_INET6;
    envoi_reception[1].sin6_port = htons(7000);
	//setip6(argv[1],&envoi_reception[3]);

	// Reception serveur
    envoi_reception[2].sin6_family = AF_INET6;
    envoi_reception[2].sin6_port = htons(8000);
	//setip6(argv[1],&envoi_reception[3]);

	// Envoi serveur
    envoi_reception[3].sin6_family = AF_INET6;
    envoi_reception[3].sin6_port = htons(9000);
	//setip6(argv[1],&envoi_reception[3]);

    // on attache l'adresse IP du serveur au socket
	// Lier socket reception client
    lier_socket6(sock[0], &envoi_reception[0]);
	
	// Lier socket envoi client
    lier_socket6(sock[1], &envoi_reception[1]);

	// Lier socket reception serveur
    lier_socket6(sock[2], &envoi_reception[2]);

	// Lier socket envoi serveur	
    lier_socket6(sock[3], &envoi_reception[3]);

    // on dit aux autres serveurs qu'on est là
       if(connexion == 1){
        // init socket serveur
        //sock_serv = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        //lier_socket6(sock_serv, &addr_server);

        // lancement d'un thread qui s'occupe de la reception des serveurs
    }



	if ( sock[0] > sock[2] ){
		max_sd = (sock[0]+1);
	}
	if ( sock[2] > sock[0] ){
		max_sd = ( sock[2]+1);
	}

    // communications du serveur
    //while(end != 1){
        
		while(end!=1){
			waitTh.tv_sec 	= 5;
			waitTh.tv_usec 	= 50;
			FD_ZERO(&read_sds);
			FD_SET(sock[0], &read_sds);
			FD_SET(sock[2], &read_sds);
			ret= select(max_sd,&read_sds,NULL, NULL, &waitTh);
			if(ret < 0){
				fprintf(stderr,"select a bugué\n");
				exit(EXIT_FAILURE);
			}
			else if(FD_ISSET(sock[0],&read_sds)){
				printf("On a recu un message du client\n");
				if(recvfrom(sock[0], buf, MESS_MAX_SIZE, 0,
            		(struct sockaddr *)&envoi_reception[0], &addrlen) == ERROR){
				    perror("recvfrom");
				    close(sock[0]);
					close(sock[1]);
					close(sock[2]);
					close(sock[3]);
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

				        envoyer_mess6(sock[1], mess, envoi_reception[0]);
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
				        printf("Demande d'arret\n");
				        if(check_access_code(hash) == 0){
				            printf("mot de passe correct\n");
				            // arret du keep alive
				        //    pthread_exit(&serveur__thread);
				            end = 1;
				        }
				        else{
				            printf("mot de passe incorrect\n");
				            end = 0;
				        }
				        free(hash);
				        break;

				    default:
				        // type de message inconnu
				        fprintf(stderr,"Erreur: message type %d inconnu\n",type_mess);
				        break;
				} // fin switch
   
        		// remise à zéro
        		memset(mess, '\0', MESS_MAX_SIZE);
        		memset(buf, '\0', MESS_MAX_SIZE);
			}
			else if(FD_ISSET(sock[2],&read_sds)){
				printf("On a recu un message du serveur\n");
			}
		}

	
        

    printf("Arret du serveur %s\n", argv[1]);
   
    // il faut notifier les autres serveurs qu'on s'arrete

    // fermeture du socket
	fermer_socket(sock[0]);
    fermer_socket(sock[1]);
	fermer_socket(sock[2]);
	fermer_socket(sock[3]);
    // suppression de la table
    supp_dht(t);

    // le programme s'est bien déroulé 
    exit(EXIT_SUCCESS);

}
