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

void supprimer_socket(int sockfd){
	if(close(sockfd)==-1){
		perror("close\n");
		exit(EXIT_FAILURE);
	}
	return;
}
