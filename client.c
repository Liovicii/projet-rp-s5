#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "dht.h"

int main(int argc, char **argv)
{
	int port_nb;
    int sockfd;
    char buf[MESS_MAX_SIZE];
    socklen_t addrlen;
	char ip6[INET6_ADDRSTRLEN];
	char type[1];
	char length[2];

	//On regarde si c'est get ou set
	if(argc<4){
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if(strcmp(argv[3],"get")==0){
		//check nb args ==5
		//On envoie juste un message
		struct sockaddr_in6 dest;

		// check the number of args on command line
		if(argc != 5)
		{
		    fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		    exit(-1);
		}

		//Conversion en entier du port
		port_nb=atoi(argv[2]);
		
		// socket factory
		if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket\n");
		exit(EXIT_FAILURE);
		}

		// init remote addr structure and other params
		dest.sin6_family = AF_INET6;
		dest.sin6_port   = htons(port_nb);
		dest.sin6_flowinfo = 0;
		addrlen         = sizeof(struct sockaddr_in6);

		printf("%s\n",argv[1]);
		// get addr from command line and convert it
		int ret;
		if((ret=inet_pton(AF_INET6,argv[1],&dest.sin6_addr)) != 1)
		{
			if (ret == 0){
				fprintf(stderr,"adresse invalide\n");
				exit(EXIT_FAILURE);
			}
		    perror("inet_pton\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
		
		// send string
		if(sendto(sockfd,argv[4],strlen(argv[4]),0,(struct sockaddr *)&dest,addrlen) == -1)
		{
		    perror("sendto\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// close the socket
		close(sockfd);
		
				//On attends une reponse
		struct sockaddr_in6 my_addr;
		struct sockaddr_in6 client;
		/*
		// check the number of args on command line
		if(argc != 2)
		{
		    printf("Usage: %s local_port\n", argv[0]);
		exit(-1);
		}
		
		port_nb=atoi(argv[1]);
		*/
		// socket factory
		if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket");
		exit(EXIT_FAILURE);
		}
	
		printf("%d\n",INADDR_ANY);
		// init local addr structure and other params
		my_addr.sin6_family      = AF_INET6;
		my_addr.sin6_port        = port_nb;
		my_addr.sin6_addr		 = in6addr_any;
		addrlen                 = sizeof(struct sockaddr_in6);
		memset(buf,'\0',MESS_MAX_SIZE);

		// bind addr structure with socket
		if(bind(sockfd,(struct sockaddr *)&my_addr,addrlen) == -1)
		{
		  perror("bind");     
		  close(sockfd);
		  exit(EXIT_FAILURE);
		}
	 	//sleep(20);
		//netstat -apu
		// reception de la chaine de caracteres
		if(recvfrom(sockfd,buf,MESS_MAX_SIZE,0,(struct sockaddr *)&client,&addrlen) == -1)
		{
		  perror("recvfrom");
		  close(sockfd);
		  exit(EXIT_FAILURE);
		}

		// print the received char
		printf("Message recu: %s\n",buf);
		printf("Longueur du message: %li\n",strlen(buf));
	
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
	if(strcmp(argv[3],"set")==0){
		//check nb args == 6
		//On envoie un message
		struct sockaddr_in6 dest;

		// check the number of args on command line
		if(argc != 6)
		{
		    printf("USAGE: %s @dest port_num string ip\n", argv[0]);
		    exit(-1);
		}
		port_nb=atoi(argv[2]);

		//On defini le type a SET
		printf("Coucou\n");
		
		int type_en=SET;
		//itoa(type_en,type,2);
		snprintf(type,2,"%d",type_en);
		//strncpy(type,,1);
		//type=(char)SET;
		printf("Type: %s\n",type);
		
		memset(buf,'\0',MESS_MAX_SIZE);
		
		//On ajoute le type a buf
		strncat(buf,type,1);
			
		//Stockage ipv6 dans ip6
		strncpy(ip6,argv[1],strlen(argv[1]));

		//On regarde si le hash est valide
		if(check_hash(argv[4])==-1){
			fprintf(stderr,"usage: le hash n'est pas assez long\n");
			exit(EXIT_FAILURE);
		}
		//on recupere la longeur du hash
		int taille_hash=strlen(argv[4]);
		snprintf(length,3,"%d",taille_hash);
		//length=(char)strlen(argv[4]);
		// On concatene la longuer a buf
		strncat(buf,length,2);
		//On concatene l'ip au buffer
		//il faudrait que l'ip fasse 46 caracteres
		strncat(buf,ip6,INET6_ADDRSTRLEN);
		//On concatene le hash au buffer
		strncat(buf,argv[4],strlen(argv[4]));
		printf("%s\n",buf);
		// socket factory
		if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket\n");
		exit(EXIT_FAILURE);
		}

		// init remote addr structure and other params
		dest.sin6_family = AF_INET6;
		dest.sin6_port   = htons(port_nb);
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
	
    
