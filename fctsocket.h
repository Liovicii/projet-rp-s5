#ifndef _FCTSOCKET_H_
#define _FCTSOCKET_H_
#include <sys/socket.h>
#define LENGTH_TYPE 1
#define LENGTH_LG 2
#define TAILLE_MAX_HASH 1000

int creer_socket(int domaine, int type, int protocole);
void fermer_socket(int sockfd);

struct sockaddr_in6 initv6(int port);
void setip6(char * ip,struct sockaddr_in6 * dest,int sockfd);
void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket);
int recevoir_mess6(int sockfd,char * str,int size,struct sockaddr_in6 socket);

void lier_socket6(int sockfd, struct sockaddr_in6 socket);

char * concatener_ip_hash(char *ip,char *hash);
void creation_chaine(char * type, char * lg,char * str, char * data);
void remplir_lg(char * ip, char * hash, char * lg);
void remplir_type(int val_type, char * type);

/*
Name: extract string 
** @param entree :
** @param sortie :
** @param indice : indice ou on commence a extraire
** @param t_a_extr : nombre d'octet que l'on veut extraire
*/
void extract_string(char * entree, char * sortie, int indice, int t_a_extr);
int get_length_ip(char * lg);
int get_length_hash(char * lg);
int get_type_from_mess(char * mes);
void extraire_taille_mess(char * lg_m,char *mes);
char * extraire_ip_mess(char * mes);
char * extraire_hash_mess(char * mes);

#endif
