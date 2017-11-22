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


#define ERROR       -1
#define IPV6_LENGTH 100


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


/**
 * \fn int port_valide (int port)
 * \brief Vérifie si un numéro de port est valide.
 *
 * \param port Un numéro de port sous forme de string.
 * \return Le numero de port si le paramètre est un numéro de port valide, -1 sinon.
 */
int port_valide(char * port);



/**
 * \fn int parse_hostname (char * hostname, char * ip)
 * \brief Détermine une adresse IPv6 pour un nom de hôte.
 *
 * \param hostname Un nom de hôte
 * \param port Numero de port
 * \param ip Pointeur où l'on stock l'adresse IP
 * \return 0 si tout se passe bien, -1 sinon.
 */
int parse_hostname(char * hostname, char * port, char * ip);




#endif
