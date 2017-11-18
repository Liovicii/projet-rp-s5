#ifndef __server_h
#define __server_h


/**
 * \enum Bool
 * \brief Défini un type booléen.
 */
typedef enum { FALSE, TRUE } Bool;


/**
 * \fn Bool ip6_valide (char * ip)
 * \brief Vérifie si une adresse IPv6 est valide.
 *
 * \param ip Chaîne de caractère contenant une IPv6
 * \return TRUE si le paramètre est une adresse IPv6 valide, FALSE sinon.
 */
Bool ip6_valide(char * ip);


/**
 * \fn Bool port_valide (int port)
 * \brief Vérifie si un numéro de port est valide.
 *
 * \param port Un numéro de port.
 * \return TRUE si le paramètre est un numéro de port valide, FALSE sinon.
 */

Bool port_valide(int port);


#endif
