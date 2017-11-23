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


#define ERROR         -1
#define MESS_MAX_SIZE 1049  // TYPE(1)+LENGTH(2)+IP(46)+DATA(1000)
#define PARTITION     26+10 // alphabet + chiffres base 10
#define TAILLE_MAX_HASH 1000 // taille max d'un hash

/* Définition des types pour les messages */
#define GET 1
#define SET 2


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
	struct ip * next;
}Ip;


/*
 * \struct hash_cel dht.h
 * \brief Liste chainée de hash.
 */
typedef struct hash_cel{
	char val[TAILLE_MAX_HASH];
	struct hash_cel * next;
	struct ip_cel * want;
	struct ip_cel * have;
}DHT;


/*
 *
 */




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



#endif
