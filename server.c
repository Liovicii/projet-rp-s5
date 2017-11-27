#include "dht.h"
#include "fctsocket.h"



/**
 * \fn void usage (char * arg)
 * \brief Affiche l'usage de la fonction sur la sortie d'erreur
 *
 * \param arg Nom du programme
 */
void usage(char * arg){
    fprintf(stderr, "usage: %s IP PORT\n", arg);
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
    
    if((strncmp("--help", arg[1], 6) == 0) && (strlen(arg[1]) == 6)){
        // option --help
        printf("Utilisation : %s IPv6 PORT\n", arg[0]);
        printf("         ou : %s --help | --version\n", arg[0]);
        printf("Lance un serveur configuré comme suit:\n");
        printf("  - adresse IPv6: argument IPv6\n");
        printf("  - numéro de port: argument PORT\n");
        exit(EXIT_SUCCESS);
    }

    if((strncmp("--version", arg[1], 9) == 0) && (strlen(arg[1]) == 9)){
        // option --version
        printf("server 1.0\n");
        printf("Copyright 2017 Lionel Jung & David Lardenois\n");
        exit(EXIT_SUCCESS);
    }
    
    // option inconnue 
    fprintf(stderr, "Erreur: option inconnue\n");
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
    int i, port, sock, type_mess, end = 0, test, nb_server = 0;
	int liste_server[MAX_SERVER];
    struct sockaddr_in6 addr_server, addr_dest;
    char buf[MESS_MAX_SIZE], mess[MESS_MAX_SIZE], lg[2], type[1];
    char *hash, *ip_m, *get;
    DHT * t = NULL;     // table des hashs


    // vérification des arguments
    if(argc != 3){
        // on regarde si on a l'option --help ou --version
        if(argc == 2){
            parse_option(argv);
        }
        usage(argv[0]);
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


    // initialisation socket
    sock = creer_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);


    // initialisation adresse IP du serveur
    addr_server.sin6_family = AF_INET6;
    addr_server.sin6_port = htons(port);


    // on attache l'adresse IP du serveur au socket
    lier_socket6(sock, addr_server);

/*
    // on dit aux autres serveurs qu'on est là
    printf("Voulez-vous vous connecter à un autre serveur: [IP]\n");
    scanf("%s", liste_server[nb_server]);
    nb_server++;
*/

    // lancement d'un ps fils qui s'occupe du keep alive

    // lancement d'un ps fils qui s'occupe de l'obsolescence des données

    // communications du serveur
    while(end != 1){
        
        recevoir_mess6(sock, buf, MESS_MAX_SIZE, addr_dest);
        printf("port reponse: %d\n", addr_dest.sin6_port);

        // affichage du message recu
        printf("Message recu:\n%s\n", buf);
       
        // analyse du message
        type_mess = get_type_from_mess(buf);
        hash = extraire_hash_mess(buf);
        ip_m = extraire_ip_mess(buf);
		printf("Machine qui m'a contacte: %s\n", ip_m);

        // on détermine ce qu'on doit faire
        switch(type_mess){
    
            case PUT:
                // message de type PUT
                if((test = put_hash(hash, ip_m, &t)) == ERROR){
                    fprintf(stderr, "put_hash failed\n");
                }
                printf("New Entry in table: IP %s has hash %s\n", ip_m, hash);
                // on doit envoyer le hash aux autres serveurs !
                if(test != NTD){
                    
                    // envoyer le hash aux serveurs voisins
                    addr_dest.sin6_family = AF_INET6;
                    addr_dest.sin6_port = htons(port);
                    remplir_type(HAVE, type);

                    for(i = 1; i < nb_server; i++){
                        //remplir_lg( ??? , hash, lg);
                        creation_chaine(type, lg, mess, hash);
                        envoyer_mess6(liste_server[i], mess, addr_dest); 
                    }
                }
                break;

            case GET:
                // message de type GET
                get = get_hash(hash, t);
                printf("GET: %s\n", get);
                if(get == NULL) break;

                // on doit envoyer un message au client
                // creation du message
                remplir_lg(ip_m, get, lg);
                remplir_type(HAVE, type);
                creation_chaine(type, lg, mess, get);

                envoyer_mess6(sock, mess, addr_dest);
                break;
          
            case NEW:
                // un nouveau serveur nous notifie
                break;

            case DECO:
                // un serveur se déconnecte
                
                break;

            case HAVE:
                // un serveur nous informe de ses modification
                printf("Reception d'une nouvelle entree\n");
                if((test = put_hash(hash, ip_m, &t)) == ERROR){
                    fprintf(stderr, "put_hash failed\n");
                }
                printf("New Entry in table: IP %s has hash %s\n", ip_m, hash);
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
        }
   
        // remise à zéro
        type_mess = 0;
        hash = NULL;
        ip_m = NULL;
        strncpy(buf, "", 0);
        strncpy(mess, "", 0);

    } // fin boucle 

    printf("Arret du serveur %s\n", argv[1]);
   
    // il faut notifier les autres serveurs qu'on s'arrete

    // fermeture du socket
    fermer_socket(sock);

    // suppression de la table
    supp_dht(t);

    // le programme s'est bien déroulé 
    exit(EXIT_SUCCESS);

}
