#ifndef _FCTSOCKET_H_
#define _FCTSOCKET_H_
#include <sys/socket.h>

int creer_socket(int domaine, int type, int protocole);
void fermer_socket(int sockfd);

struct sockaddr_in6 initv6(int port);
void setip6(char * ip,struct sockaddr_in6 * dest,int sockfd);
void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket);
int recevoir_mess6(int sockfd,char * str,int size,struct sockaddr_in6 socket);

void lier_socket6(int sockfd, struct sockaddr_in6 socket);
#endif
