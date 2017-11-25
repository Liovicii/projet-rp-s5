/**
 *\file fctsocket.h
 *\author David.Lardenois Lionel.Jung
 */

#ifndef _FCTSOCKET_H_
#define _FCTSOCKET_H_
#include <sys/socket.h>
#define LENGTH_TYPE 1
#define LENGTH_LG 2
#define TAILLE_MAX_HASH 1000

/**
 *\fn int creer_socket(int domaine, int type, int protocole)
 *\brief fonction pour creer un socket
 *\param domaine domaine de l'adresse ip du socket
 *\param type type de donnée que l'on veut envoyer
 *\param protocole protocole UDP ou TCP
 *\return retourne le descripteur de fichier si tous s'est bien passé
 */
int creer_socket(int domaine, int type, int protocole);

/**
 *\fn void fermer_socket(int sockfd)
 *\brief ferme le socket donné en argument
 *\param sockfd desripteur de fichier du socket
 *\return retourne rien si tous s'est bien passé
 */
void fermer_socket(int sockfd);

/**
 *\fn struct sockaddr_in6 initv6(int port)
 *\brief rempli et renvoie une structure sockaddr_in6
 *\param port numero de port lié a la structure
 *\return retourne une structure si tous s'est bien passé
*/
struct sockaddr_in6 initv6(int port);

/**
 *\fn void setip6(char * ip,struct sockaddr_in6 * dest,int sockfd)
 *\brief initialise l'ip d'un structure sockaddr_in6
 *\param ip ip a inserer dans la structure
 *\param dest structure qui va recevoir l'ip
 *\param sockfd descripteur de fichier du socket
 *\return retourne rien si tous s'est bien passé
*/
void setip6(char * ip,struct sockaddr_in6 * dest,int sockfd);

/**
 *\fn void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket)
 *\brief envoie un message pour les adresses ipv6
 *\param sockfd descripteur de fichier du socket
 *\param str chaine de caracter que l'on va envoyer
 *\param socket info de l'envoyeur
 *\return retourne rien si tous s'est bien passé
*/
void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket);

/**
 *\fn int recevoir_mess6(int sockfd,char * str,int size,struct sockaddr_in6 socket)
 *\brief recoit un message pour les adresses ipv6
 *\param sockfd desripteur de fichier du socket ou l'on veut recevoir
 *\param str chaine de caractere dans laquelle on va stocker le message
 *\param size taille max d"octet que l'on peut stocker
 *\param socket 
 *\return retourne le nobre de caractere recu si tous se passe bien
*/
int recevoir_mess6(int sockfd,char * str,int size,struct sockaddr_in6 socket);

/**
 *\fn void lier_socket6(int sockfd, struct sockaddr_in6 socket
 *\brief lie une structure sockaddr_in6 a un socket
 *\param sockfd desctipteur de fichier du socket auquel on veut se lier
 *\param socket structure qu'on veut lier au socket
 *\return retourne rien si tous s'est bien passé
*/
void lier_socket6(int sockfd, struct sockaddr_in6 socket);

/**
 *\fn char * concatener_ip_hash(char *ip,char *hash)
 *\brief concatene l'ip et le hash dans une meme chaine
 *\param ip chaine de caractere ip
 *\param hash chaine de caractere hash
 *\return retourne la concatenation de ip et hash
*/
char * concatener_ip_hash(char *ip,char *hash);

/**
 *\fn void creation_chaine(char * type, char * lg,char * str, char * data)
 *\brief creation du message qu'on envera
 *\param type valeure de type
 *\param lg chaine de caracter contenant la longeur
 *\param str chaine de caracter contenant les données
 *\param data chaine de caracter ou tout concatener
 *\return retourne rien si tout s'est bien passé
*/
void creation_chaine(char * type, char * lg,char * str, char * data);

/**
 *\fn void remplir_lg(char * ip, char * hash, char * lg)
 *\brief	rempli la longueur avec la taille de l'ip et du hash 
 			chacun dans un octet
 *\param ip chaine de caractere contenant ip
 *\param hash chaine de caractere contenant hash
 *\param lg chaine de caracter qui va contenir les deux tailles
 *\return retourne rien si tous s'est bien passé
*/
void remplir_lg(char * ip, char * hash, char * lg);

/**
 *\fn remplir_type(int val_type, char * type)
 *\brief on rempli le type 
 *\param val_type valeure tu type a mettre dans type
 *\param type chaine de caractere qui va contenir type
 *\return retorune rien si tous s'est bien passé
*/
void remplir_type(int val_type, char * type);

/**
 *\fn extract_string(char * entree, char * sortie, int indice, int t_a_extr)
 *\brief Fonction d'extraction de chaine de caractere
 *\param entree chaine a partir de laquelle on veut extraire 
 *\param sortie chaine qui va contenir ce qu'on extrait
 *\param indice indice a partir duquel on va commencer a extraire dans la chaine
 *\param t_a_extr nombre d'octets qu'on veut extraire de la chaine de caractère
 *\return void
*/
void extract_string(char * entree, char * sortie, int indice, int t_a_extr);

/**
 *\fn get_length_ip(char * lg)
 *\brief extrait la taille de l'ip dans la chaine lg
 *\param lg chaine ou on veut extraire la taille de l'ip
 *\return 
*/
int get_length_ip(char * lg);

/**
 *\fn get_length_hash(char * lg)
 *\brief extrait la taille du hash dand la chaine lg
 *\param lg chaine ou on veut extraire la taille du hash
 *\return 
*/
int get_length_hash(char * lg);

/**
 *\fn get_type_from_mess(char * mes)
 *\brief converti le type en entier
 *\param mes message recu 
 *\return retourne le type du message 
*/
int get_type_from_mess(char * mes);

/**
 *\fn extraire_taille_mess(char * lg_m,char *mes)
 *\brief extrait la taille que fait le message contenu dans mes
 *\param lg_m chaine de caracter qui va contenir la longeur de ip et hash
 *\param mes message qu'on a recu
 *\return retourne rien si tout s'est bien passé
*/
void extraire_taille_mess(char * lg_m,char *mes);

/**
 *\fn char * extraire_ip_mess(char * mes)
 *\brief extrait la chaine de caracter contenant l'ip du message recu
 *\param mes message recu
 *\return retourne chaine de caractere extraite
*/
char * extraire_ip_mess(char * mes);

/**
 *\fn char * extraire_hash_mess(char * mes)
 *\brief extrait la chaine de caracter contenant le hash du message recu
 *\param mes message recu
 *\return retourne chaine de caractere extraite
*/
char * extraire_hash_mess(char * mes);

#endif
