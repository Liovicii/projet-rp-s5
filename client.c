#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "dht.h"
#include "fctsocket.h"

int main(int argc, char **argv)
{
    int sockfd;
    char buf[MESS_MAX_SIZE];
    socklen_t addrlen;
	char ip6[INET6_ADDRSTRLEN];
	char type[2];
	char length[3];

	//On regarde si c'est get ou set
	if(argc<4){
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if(strcmp(argv[3],"get")==0){
		
		//On envoie juste un message
		struct sockaddr_in6 dest;

		// On verifie le nombre d'arguments
		if(argc != 5)
		{
		    fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		    exit(-1);
		}

		// On creer le socket ipV6
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
		// on initialise la structure
		dest=initv6(atoi(argv[2]));
		//On initialise addrlen
		// On initialise l'ip du socket
		setip6(argv[1],dest,sockfd);
		// On envoie le message
		printf("On va envoyer le hash\n");
		envoyer_mess6(sockfd,argv[4],dest);
		fermer_socket(sockfd);
		printf("On a envoyé le hash\n");
		
		//On attends une reponse
		struct sockaddr_in6 my_addr;
		struct sockaddr_in6 client;
		// On initilise le socket
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
		my_addr=initv6(atoi(argv[2]));
		setip6("in6addr_any",my_addr,sockfd);
		
		memset(buf,'\0',MESS_MAX_SIZE);
		//On lie la structure au socket
		lier_socket6(sockfd, my_addr);
		
		// reception de la chaine de caracteres
		printf("On attends de recvoir un message\n");
		int rec=recevoir_mess6(sockfd,buf,MESS_MAX_SIZE,client);
		printf("On a recu la reponse\n");
		// print the received char
		printf("Message recu: %s\n",buf);
		printf("Longueur du message: %d\n",rec);
	
		char adr_ip[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET6,&client.sin6_addr,adr_ip,INET_ADDRSTRLEN)==NULL){
			perror("inet_ntop\n");
			exit(EXIT_FAILURE);		
		}
		printf("Ip source: %s\n",adr_ip);
		printf("Numero de port de l'expediteur: %d\n",client.sin6_port);
		// close the socket
		close(sockfd);
	}
	if(strcmp(argv[3],"put")==0){
		//check nb args == 6
		//On envoie un message
		struct sockaddr_in6 dest;

		// check the number of args on command line
		if(argc != 6)
		{
		    printf("USAGE: %s @dest port_num put hash ip\n", argv[0]);
		    exit(-1);
		}
				
		int type_en=SET;
		//itoa(type_en,type,2);
		snprintf(type,2,"%d",type_en);
		type[1]=('\0');
	
		memset(buf,'\0',MESS_MAX_SIZE);

			
		//Stockage ipv6 dans ip6
		int taille_ip=strlen(argv[1]);
		strncpy(ip6,argv[1],INET6_ADDRSTRLEN);
		//On regarde si le hash est valide
		if(check_hash(argv[4])==-1){
			fprintf(stderr,"usage: le hash n'est pas assez long\n");
			exit(EXIT_FAILURE);
		}
		//on recupere la longeur du hash
		int taille_hash=strlen(argv[4]);
		
		//length=(char)strlen(argv[4]);
		//snprintf(&length[0],2,"%c",taille_ip);
		//snprintf(&length[1],2,"%c",taille_hash);
		length[0]=(char)taille_ip-'0';
		length[1]=(char)taille_hash-'0';
		length[2]='\0';
		
		
		//On ajoute le type a buf
		strncpy(buf,type,strlen(type));
		// On concatene la longueur a buf
		strncat(buf,length,strlen(length));
		//On concatene l'ip au buffer
		//il faudrait que l'ip fasse 46 caracteres
		//printf("Message: %s\n",ip6);
		strncat(buf,ip6,INET6_ADDRSTRLEN);
		//On concatene le hash au buffer
		strncat(buf,argv[4],strlen(argv[4]));
		printf("%s\n",buf);

		char recup[3];
		strncpy(recup,buf+1,2);
		recup[3]='\0';
		if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket\n");
		exit(EXIT_FAILURE);
		}

		// init remote addr structure and other params
		dest.sin6_family = AF_INET6;
		dest.sin6_port   = htons(atoi(argv[2]));
		addrlen         = sizeof(struct sockaddr_in6);
		
		// get addr from command line and convert it
		if(inet_pton(AF_INET6,argv[1],&dest.sin6_addr) != 1)
		{
		    perror("inet_pton\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	
		// send string
		if(sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&dest,addrlen) == -1)
		{
		    perror("sendto\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// close the socket
		close(sockfd);
	}
	else{
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
	
    
