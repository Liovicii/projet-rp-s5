#ifndef __server_h
#define __server_h

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ERROR -1

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
 * \fn Bool ip6_valide (char * ip)
 * \brief Vérifie si une adresse IPv6 est valide.
 *
 * \param ip Chaîne de caractère contenant une IPv6
 * \return TRUE si le paramètre est une adresse IPv6 valide, FALSE sinon.
 */
char * ip6_valide(char * ip);


/**
 * \fn Bool port_valide (int port)
 * \brief Vérifie si un numéro de port est valide.
 *
 * \param port Un numéro de port.
 * \return TRUE si le paramètre est un numéro de port valide, FALSE sinon.
 */
int port_valide(int port);


#endif
