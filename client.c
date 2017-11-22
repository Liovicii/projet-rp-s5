#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int port_nb;
    int sockfd;
    char buf[1024];
    socklen_t addrlen;

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
		memset(buf,'\0',1024);

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
		if(recvfrom(sockfd,buf,1024,0,(struct sockaddr *)&client,&addrlen) == -1)
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
		    printf("USAGE: %s @dest port_num string\n", argv[0]);
		    exit(-1);
		}
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
		addrlen         = sizeof(struct sockaddr_in6);

		// get addr from command line and convert it
		if(inet_pton(AF_INET6,argv[1],&dest.sin6_addr) != 1)
		{
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
	}
	else{
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
	
    
