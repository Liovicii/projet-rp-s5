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
    int port, sock, type_mess;
    struct sockaddr_in6 addr_server, addr_dest;
    char buf[MESS_MAX_SIZE];
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

    // communications du serveur 
    recevoir_mess6(sock, buf, MESS_MAX_SIZE, addr_server);
     
    // affichage du message recu
    printf("Message recu:\n%s\n", buf);

    type_mess = get_type_from_mess(buf);
    hash = extraire_hash_mess(buf);
    ip_m = extraire_ip_mess(buf);

    // on détermine ce qu'on doit faire
    switch(type_mess){

        case PUT:
            // message de type PUT
            if(put_hash(hash, ip_m, &t) == ERROR){
                fprintf(stderr, "put_hash failed\n");
            }
            printf("IP %s added to hash %s\n", ip_m, hash);
            break;

        case GET:
            // message de type GET
            get = get_hash(hash, t);
            printf("GET: %s\n", get);
            
            // on doit envoyer un message au client
            addr_dest.sin6_family = AF_INET6;
            addr_dest.sin6_port = htons(port);
            setip6(ip_m, &addr_dest, sock);
            
            envoyer_mess6(sock, get, addr_dest);
            break;

        default:
            // type de message inconnu
            fprintf(stderr, "Erreur: message de type %d inconnu\n", type_mess);
            break;
    }


    // fermeture du socket
    fermer_socket(sock);

    // le programme s'est bien déroulé 
    return EXIT_SUCCESS;

}
