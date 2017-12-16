#include "dht.h"
#include "fctsocket.h"
#include <signal.h>



volatile sig_atomic_t sigInt=0;

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

void Sortir(__attribute__((unused))int i){
	sigInt=1;
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
    //fprintf(stderr, "Erreur: option inconnue\n");
    usage(arg[0]);
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
    int port;
	int type_mess;
	int end = 0;
	int test;
    int connexion = 0, nb_server = 0;
	//int sock_serv;
    //int sock_serv[10];
	int sock[4];
	struct sockaddr_in6 envoi_reception[4];
	// Pour le keep alive
	struct sockaddr_in6 alive;
	int sock_alive;
	struct timeval waitTh;	
	int max_sd;
	fd_set read_sds;
	int ret;

    socklen_t addrlen = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 liste_server[MAX_SERVER];
    
    char buf[MESS_MAX_SIZE], mess[MESS_MAX_SIZE], lg[3], type[2];
    char *hash = NULL;
	char *ip_m = NULL;
	char *get = NULL;
	
	//CREATION DE LA TABLE DE HASH  
    DHT * t = NULL; 

    // CREATION DES THREADS
    pthread_t keep_alive_thread;
    pthread_t deco_serveur;
   	struct ka_data thread_arg;
	
	// GESTION DU SIGNAL
	struct sigaction actionExit;
	actionExit.sa_handler=Sortir;
    actionExit.sa_flags = 0;
	sigemptyset(&actionExit.sa_mask);
	sigaddset(&actionExit.sa_mask, SIGQUIT);

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
                fprintf(stderr, "Option inconnue\n");
                usage(argv[0]);
            }
            add_server(liste_server, argv[4], PORT_REC_SERV, &nb_server);
            connexion = 1;
            break;
        default:
            // erreur nombre d'arguments
            usage(argv[0]);
            break;
    }


    // vérification du port
    if((port = port_valide(atoi(argv[2]))) == ERROR){
        usage(argv[0]);
    }


    // convertir l'argument en adresse IPv6
    //Init struct reception client
    if(convert_ipv6(argv[1], atoi(argv[2]), &envoi_reception[0]) == ERROR){
        usage(argv[0]);
    } 
	//Init struct envoi client
    if(convert_ipv6(argv[1], PORT_ENV_CLIENT, &envoi_reception[1]) == ERROR){
        usage(argv[0]);
    } 

	//Init struct reception serveur
    if(convert_ipv6(argv[1], PORT_REC_SERV, &envoi_reception[2]) == ERROR){
        usage(argv[0]);
    } 

	//Init struct envoi serveur
    if(convert_ipv6(argv[1], PORT_ENV_SERV, &envoi_reception[3]) == ERROR){
        usage(argv[0]);
    } 

	// Init struct keep_alive
    if(convert_ipv6(argv[1], PORT_KEEP_ALIVE, &alive) == ERROR){
        usage(argv[0]);
    } 
    // initialisation socket client
    sock[0] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[1] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[2] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock[3] = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	sock_alive = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    // on attache l'adresse IP du serveur au socket
    lier_socket6(sock_alive, &alive);
	// Lier socket reception client
    lier_socket6(sock[0], &envoi_reception[0]);
	
	// Lier socket envoi client
    lier_socket6(sock[1], &envoi_reception[1]);

	// Lier socket reception serveur
    lier_socket6(sock[2], &envoi_reception[2]);

	// Lier socket envoi serveur	
    lier_socket6(sock[3], &envoi_reception[3]);


    /**		INITIALISATION DE LA STRUCTURE POUR LES THREADS**/
   	thread_arg.sockfd=sock_alive;
   	thread_arg.nb_serv=&nb_server;
   	thread_arg.liste=liste_server;
   	thread_arg.exit=&end;
   	pthread_mutex_init(&thread_arg.acces_serveurs,NULL);
   	pthread_mutex_init(&thread_arg.acces_liste,NULL);
   	pthread_mutex_init(&thread_arg.acces_table,NULL);
   	


    // on dit aux autres serveurs qu'on est là
       if(connexion == 1){
		// On veut dire qu'on veut se connecter
		remplir_type(NEW,type);
		//On envoie le message au serveur
		envoyer_mess6(sock[2],type,liste_server[0]);
		//On attends la reponse
		max_sd=sock[2]+1;
		waitTh.tv_sec 	= 5;
		waitTh.tv_usec 	= 50;
		FD_ZERO(&read_sds);
		FD_SET(sock[2], &read_sds);
		ret= select(max_sd,&read_sds,NULL, NULL, &waitTh);
		if (ret <0){
			perror("select\n");
			exit(EXIT_FAILURE);
		}
		else if (FD_ISSET(sock[2],&read_sds)){
			memset(buf,'\0',MESS_MAX_SIZE);
			recevoir_mess6(sock[2],buf,MESS_MAX_SIZE,envoi_reception[2]);
			extract_string(buf,type,0,LENGTH_TYPE);
			type_mess = get_type_from_mess(buf);
			//On analyse la reponse
			if(type_mess == ERROR){
				//Si la reponse est negative on sort
				fprintf(stderr,"Not enough space for new connexion\n");
				close(sock[0]);
				close(sock[1]);
				close(sock[2]);
				close(sock[3]);
				close(sock_alive);
				supp_dht(t);
				end=1;
				exit(EXIT_FAILURE);	
			}
		}
		else {
			fprintf(stderr,"NO ANSWER FROM SERVER\n");
			exit(EXIT_FAILURE);
		}
    }
    printf("Le serveur est operationnel\n");
    //On lance le thread de keep alive
	pthread_create(&keep_alive_thread,NULL,keep_alive,&thread_arg);

	sigaction(SIGINT,&actionExit,NULL);
	if ( sock[0] > sock[2] ){
		max_sd = (sock[0]+1);
	}
	if ( sock[2] > sock[0] ){
		max_sd = ( sock[2]+1);
	}

    // communications du serveur
    //while(end != 1){
        
		while(end!=1){
	
			//Reception des messages
			waitTh.tv_sec 	= 5;
			waitTh.tv_usec 	= 50;
			FD_ZERO(&read_sds);
			FD_SET(sock[0], &read_sds);
			FD_SET(sock[2], &read_sds);
			ret= select(max_sd,&read_sds,NULL, NULL, &waitTh);
            if(ret < 0){
            	if(sigInt==1){
					printf("On quitte le serveur\n");
					//On notifie tous les serveur qu'on s'est deconnecté
					sendto_all_servs(sock[1],DECO,"",&nb_server,liste_server);
				
					close(sock[0]);
					close(sock[1]);
					close(sock[2]);
					close(sock[3]);
					close(sock_alive);
					end=1;
					supp_dht(t);
					exit(EXIT_SUCCESS);
				}
				perror("select");
				exit(EXIT_FAILURE);
			}
			else if(FD_ISSET(sock[0],&read_sds)){
				//On recupere le message que le client nous a envoyé
				if(recvfrom(sock[0], buf, MESS_MAX_SIZE, 0,
            		(struct sockaddr *)&envoi_reception[0], &addrlen) == ERROR){
				    perror("recvfrom");
				    close(sock[0]);
					close(sock[1]);
					close(sock[2]);
					close(sock[3]);
					end=1;
					close(sock_alive);
					supp_dht(t);
				    exit(EXIT_FAILURE);
				}

				
				type_mess = get_type_from_mess(buf);
				hash = extraire_hash_mess(buf);
				// analyse du message
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
                           	// Envoyer have a tous les serveur
                            sendto_all_servs(sock[1],HAVE,buf,&nb_server,liste_server);
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
				        // on doit envoyer un message au client
				        // creation du message
				        remplir_lg("", get, lg);
				        remplir_type(HAVE, type);
				        creation_chaine(type, lg, get, mess);

				        envoyer_mess6(sock[1], mess, envoi_reception[0]);
				        free(get);
				        free(hash);
				        break;
				    case EXIT:
				        // on demande au serveur de s'arreter
				        // on vérifie le code d'acces
				        printf("Demande d'arret\n");
				        if(check_access_code(hash) == 0){
				            printf("mot de passe correct\n");
				            end = 1;
				        }
				        else{
				            printf("mot de passe incorrect\n");
				            end = 0;
				        }
				        free(hash);
				        //On notifie tous les serveur qu'on va se deconnecter
						sendto_all_servs(sock[1],HAVE,buf,&nb_server,liste_server);
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
				if(recvfrom(sock[2], buf, MESS_MAX_SIZE, 0,
            		(struct sockaddr *)&envoi_reception[2], &addrlen) == ERROR){
				    perror("recvfrom");
				    //Erreur de reception on quitte tout
				    close(sock[0]);
					close(sock[1]);
					close(sock[2]);
					close(sock[3]);
					end=1;
					supp_dht(t);
					close(sock_alive);
				    exit(EXIT_FAILURE);
				}
				// analyse du message
				type_mess = get_type_from_mess(buf);
				hash = extraire_hash_mess(buf);
				// on détermine ce qu'on doit faire
				switch(type_mess){
		
				    case HAVE:
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
				  
				    case NEW:
				    	printf("On recoit une demande de connexion ou un nouveau serveur a ajouter\n");
				        if(nb_server>9){
                            printf("Not enough space for new connexion\n");
				        	// Il n'y a plus de place dans la liste
				        	// On dit qu'il n'y a plus de place
				        	remplir_type(ERROR,type);
				        	envoyer_mess6(sock[3], mess, envoi_reception[2]);
				        }
				        else{
				        	// Il y a de la place
				        	// On repond au serveur qu'il y a de la place
				        	liste_server[nb_server]=envoi_reception[2];
				        	remplir_type(YES,type);
				        	envoyer_mess6(sock[3],type,envoi_reception[2]);
				        	//On lui envoie la table de hash
				        	send_hash_table(sock[3],&liste_server[nb_server],t);
				        	//On invremente le nombre de serveurs
				        	nb_server++;
				        	//On lui partage les serveurs connus
				        	send_server_list(sock[3],liste_server,&nb_server);
				        }
				        break;
                    case NEW_SERV:
                            //On extrait l'ip du message
                            ip_m = extraire_ip_mess(buf);
                            //On creer une entree pour stocker les coordonnées du serveur
                            if(convert_ipv6(ip_m, PORT_ENV_SERV, &liste_server[nb_server]) == ERROR){
                                usage(argv[0]);
                            } 
                            //On fini de remplir la structure
                        	liste_server[nb_server].sin6_family=AF_INET6;
                            //On rempli le port de communication par defaut
                            liste_server[nb_server].sin6_port=htons(PORT_ENV_SERV);
				        	nb_server++;
                        break;
				    case DECO:
				    	//On met l'ip a supprimer de la liste dans la structure
				    	thread_arg.ip_deco=envoi_reception[2];
				    	//On lance le thread
				    	pthread_create(&deco_serveur,NULL,deconnexion_serv,&thread_arg);
				        break;
					case KEEP_ALIVE:
						//On reponds par yes quand on recoit un message
						remplir_type(YES,type);
               			envoyer_mess6(sock_alive,type,envoi_reception[2]);
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
			//Tant qu'on recoit rien on refait un tour dans la boucle
			else{}
			
		}

	
        

    printf("Arret du serveur %s\n", argv[1]);
   
    // il faut notifier les autres serveurs qu'on s'arrete

    // fermeture du socket
	fermer_socket(sock[0]);
    fermer_socket(sock[1]);
	fermer_socket(sock[2]);
	fermer_socket(sock[3]);
	fermer_socket(sock_alive);
    // suppression de la table
    supp_dht(t);

    // le programme s'est bien déroulé 
    exit(EXIT_SUCCESS);

}
