#include "fctsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int creer_socket(int domaine, int type, int protocole){
	int sockfd;
	if((sockfd = socket(domaine,type,protocole)) == -1)
	{
	    perror("socket\n");
		exit(EXIT_FAILURE);
	}
	return sockfd; 
}

void fermer_socket(int sockfd){
	if(close(sockfd)==-1){
		perror("close\n");
		exit(EXIT_FAILURE);
	}
	return;
}

struct sockaddr_in6 initv6(int port){
	struct sockaddr_in6 dest;
	dest.sin6_family = AF_INET6;
	dest.sin6_port   = htons(port);
	return dest;
}

void setip6(char * ip,struct sockaddr_in6 dest,int sockfd){
	int ret;
	if((ret=inet_pton(AF_INET6,ip,&dest.sin6_addr)) != 1)
	{
		if (ret == 0){
			fprintf(stderr,"adresse invalide\n");
			exit(EXIT_FAILURE);
		}
	    perror("inet_pton\n");
		fermer_socket(sockfd);
		exit(EXIT_FAILURE);
	}
	return;
}

void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket,int addrlen){
	if(sendto(sockfd,str,strlen(str),0,(struct sockaddr *)&socket,addrlen) == -1)
	{
		perror("sendto\n");
		fermer_socket(sockfd);
		exit(EXIT_FAILURE);
	}

	return;
}
