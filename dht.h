#ifndef __server_h
#define __server_h

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>


#define ERROR           -1    // Retour de fonction pour les erreurs
#define NTD             2     // Retour de fonction Nothing to do
#define MESS_MAX_SIZE   1049  // TYPE(1)+LENGTH(2)+IP(46)+DATA(1000)
#define TAILLE_MAX_HASH 1000  // taille max d'un hash
#define MAX_IPS         10    // nb max d'adresses IP à envoyer par le serveur
#define MAX_SERVER      10    // nombre max de serveur qu'on peut connaitre
#define TIME_LIMIT      60     // obsolescence après 30 secondes

#define PASS            "password123" // mot de passe serveur 

/* Définition des types pour les messages */
enum {GET, PUT, WANT, HAVE, EXIT, NEW, DECO, KEEP_ALIVE, YES, NO};

/**
 * \def PERRORMSG(cmd, msg)
 * \brief Test une primitives systèmes.
 *
 * \param cmd La primitives systèmes qui est exécuté.
 * \param msg Le message que doit afficher perror.
 */
#define PERRORMSG(cmd, msg) if((cmd) == ERROR) \
    do{perror(msg); exit(EXIT_FAILURE);}while(0);


/*
 * \struct ip_cel dht.h
 * \brief Liste chainée de IP
 */
typedef struct ip_cel{
    char val[INET6_ADDRSTRLEN];
    struct ip_cel * next;
}IP;


/*
 * \struct hash_cel dht.h
 * \brief Table de hash.
 */
typedef struct hash_cel{
    char val[TAILLE_MAX_HASH];
    struct hash_cel * next;
    struct ip_cel * want;
    struct ip_cel * have;
    time_t maj;
}DHT;

/*
 * \struct ka_data dht.h
 * \brief Contient les donnés nécessaire au thread chargé keep alive
 */
struct ka_data{
    int sockfd;
    int nb_serv;
    pthread_mutex_t acces_table;
    pthread_mutex_t acces_liste;
    struct sockaddr_in6 * liste;
};


/**
 * \fn int port_valide (int port)
 * \brief Vérifie si un numéro de port est valide.
 *
 * \param port Un numéro de port sous forme de string.
 * \return Le numero de port si le paramètre est 
 *  un numéro de port valide, -1 sinon.
 */
int port_valide(char * port);



/**
 * \fn int parse_hostname (char * hostname, char * ip)
 * \brief Détermine une adresse IPv6 pour un nom de hôte.
 *
 * \param hostname Un nom de hôte à résoudre
 * \param port Numero de port (en string)
 * \param ip Pointeur où l'on stock l'adresse IP
 * \return 0 si tout se passe bien, -1 sinon.
 */
int parse_hostname(char * hostname, char * port, char * ip);



/**
 * \fn convert_ipv6 (char * arg_ip, char * arg_port, struct sockaddr_in6 * addr)
 * \brief Vérifie qu'une adresse IPv6 en format char* est valide et la stock
 *  dans la structure addr.
 *
 * \param arg_ip Adresse IPv6 entrée par l'utilisateur.
 * \param arg_port Numéro de port entré par l'utilisateur.
 * \param addr Structure dans laquelle on stock l'adresse IP.
 * \return 0 si tout se passe bien, -1 sinon.
 */
int convert_ipv6(char * arg_ip, char * arg_port, struct sockaddr_in6 * addr);



/* 
 * \fn int check_hash (char * hash)
 * \brief Verifie qu'un hash est valide
 *
 * \param hash un hash au format string
 * \return 0 si le hash est valide, -1 sinon
 */
int check_hash(char * hash);


/*
 * \fn int check_access_code
 * \brief Vérifie si le code entré est valide
 *  permet d'acceder au serveur pour des manipulation critique
 *  (terminer le serveur par exemple)
 *
 * \param code Le mot de passe qu'on veut tester
 * \return 0 si le code est juste, -1 sinon
 */
int check_access_code(char * code);


/*
 * \fn void supp_server (struct sockaddr_in6 * liste, int i, int *nb)
 * \brief Supprimer le ieme élement de la liste "liste"
 *
 * \param liste Une liste d'adresse IP de serveur
 * \param i L'élément de la liste à supprimer
 * \param nb Le nombre d'éléments de la liste
 */
void supp_server(struct sockaddr_in6 * liste, int i, int *nb);


/*
 * \fn void add_server(struct sockaddr_in6*liste,char*ip,char*port,int*nb)
 * \breif Ajoute l'adresse d'un serveur dans une liste de serveur
 *
 * \param liste Une liste d'adresse de serveur
 * \param ip L'ip du serveur à ajouter
 * \param port Le port du serveur à ajouter
 * \nb Le nombre de serveur déjà dans la liste
 */
void add_server(struct sockaddr_in6*liste,char*ip,char*port,int*nb);





/* * *  TABLE DES HASHS * * */




/*
 * \fn DHT * init_dht (char * hash)
 * \brief Initialise une table pour stocker les hash
 *
 * \param hash Le premier hash de la table
 * \return Un pointeur vers la table créée
 */
DHT * init_dht(char * hash);



/*
 * \fn void supp_dht (DHT * table)
 * \brief Supprime une table de hash
 *
 * \param table Un pointeur vers une table
 */
void supp_dht(DHT * table);



/*
 * \fn void affiche_dht (DHT * table)
 * \brief Affiche les éléments d'un table
 *
 * \param table Un pointeur vers une table
 */
void affiche_dht (DHT * table);



/*
 * \fn char * get_hash (char * hash, DHT * table)
 * \brief Recupère les adresses IP qui possède un hash
 *
 * \param hash Un hash
 * \param table Un pointeur vers une table
 * \return Une chaine de caractère contenant la liste des IPs
 *         la forme de la chaine est " IP IP... IP"
 */
char * get_hash(char * hash, DHT * table);


/*
 * \fn int put_hash (char * hash, char * ip, DHT * table;
 * \brief Ajoute une adresse IP qui possède un hash
 *
 * \param hash Le hash associé à l'adresse IP
 * \param ip L'adresse IP de la machine qui possède le hash
 * \param table La table dans laquelle on stockera l'entrée
 * \return 0 si tu se passe bien, -1 en cas d'erreur, 2 si 
 *  l'adresse IP est déjà présente dans la liste.
 */
int put_hash(char * hash, char * ip, DHT ** table);



/*
 * \fn void delete_hash (char * hash, DHT * table)
 * \brief supprime un hash d'une table
 *
 * \param hash un hash
 * \param table une table de hash
 */
void delete_hash(char * hash, DHT ** table);


/*
 * \fn void delete_ip (char * hash, char * ip, DHT * table, int liste)
 * \brief Supprime une IP associé à un hash selon 
 *
 * \param hash un hash sous forme de string
 * \param ip une ip sous forme de string
 * \param table un pointeur vers une table de hash
 * \param liste la liste dans laquelle se trouve l'IP (WANT ou HAVE)
 */
void delete_ip(char * hash, char * ip, DHT * table, int liste);

/* * *  MESSAGES ENVOYES AU SERVEURS * * */

/*
 * \fn void void send_hash_table(int sockfd, struct sockaddr_in6 * recepteur, DHT * table)
 * \brief Envoie la table de hash au serveur 
 * 
 * \param sockfd, socket a partir duquel on va envoyer le message
 * \param recepteur serveur a qui on va envoyer le message
 * table table de hash
 */
void send_hash_table(int sockfd, struct sockaddr_in6 * recepteur, DHT * table);

/*
 * \fn void supprimer_serveur(int indice, int * serveurs, struct sockaddr_in6 * liste)
 * \brief On supprimer un serveur de la liste de serveurs 
 * 
 * \param indice, indice du serveur qu'on doit supprimer de la liste
 * \param serveur liste de socket des serveurs
 * table liste liste des ip et des ports de destination des serveurs
 */
void supprimer_serveur(int indice, int * serveurs, struct sockaddr_in6 * liste);

/*
 * \fn void inserer_serveur(int * nb_serveur, struct sockaddr_in6 * recepteur, int * serveurs, struct sockaddr_in6 * liste)
 * \brief On insere un serveur dans la liste de serveurs
 * 
 * \param nb_serveur nombre de serveurs dans la liste
 * \param recepteur serveur que l'on va inserer
 * \param serveurs liste de socket associes au serveurs
 * \param liste liste de sockaddr_in6 des serveurs
 */
void inserer_serveur(int * nb_serveur, struct sockaddr_in6 * recepteur, int * serveurs, struct sockaddr_in6 * liste);

/*
 * \fn void keep_alive(int *nb_serveur, struct sockaddr_in6 * liste, int * serveurs)
 * \brief On fait un keep alive des serveurs
 * 
 * \param nb_serveur nombre de serveurs dans la liste
 * \param liste de sockaddr_in6 des serveurs
 * \param serveurs liste de socket associes au serveurs
 */
void keep_alive(int *nb_serveur, struct sockaddr_in6 * liste, int * serveurs);


#endif
