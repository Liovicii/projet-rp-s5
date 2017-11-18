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
 * \fn Bool port_valide (int port)
 * \brief Vérifie si un numéro de port est valide.
 *
 * \param port Un numéro de port.
 * \return TRUE si le paramètre est un numéro de port valide, FALSE sinon.
 */
int port_valide(int port);


#endif
