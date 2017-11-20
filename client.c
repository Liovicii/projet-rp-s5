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
		struct sockaddr_in dest;

		// check the number of args on command line
		if(argc != 4)
		{
		    printf("USAGE: %s @dest port_num string\n", argv[0]);
		    exit(-1);
		}
		port_nb=atoi(argv[2]);
		// socket factory
		if((sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket\n");
		exit(EXIT_FAILURE);
		}

		// init remote addr structure and other params
		dest.sin_family = AF_INET;
		dest.sin_port   = port_nb;
		addrlen         = sizeof(struct sockaddr_in);

		// get addr from command line and convert it
		if(inet_pton(AF_INET,argv[1],&dest.sin_addr) != 1)
		{
		    perror("inet_pton\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	
		// send string
		if(sendto(sockfd,argv[3],strlen(argv[3]),0,(struct sockaddr *)&dest,addrlen) == -1)
		{
		    perror("sendto\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// close the socket
		close(sockfd);
	}
	if(strcmp(argv[3],"set")==0){
		//check nb args == 6
		//On envoie un message
		struct sockaddr_in dest;

		// check the number of args on command line
		if(argc != 4)
		{
		    printf("USAGE: %s @dest port_num string\n", argv[0]);
		    exit(-1);
		}
		port_nb=atoi(argv[2]);
		// socket factory
		if((sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket\n");
		exit(EXIT_FAILURE);
		}

		// init remote addr structure and other params
		dest.sin_family = AF_INET;
		dest.sin_port   = port_nb;
		addrlen         = sizeof(struct sockaddr_in);

		// get addr from command line and convert it
		if(inet_pton(AF_INET,argv[1],&dest.sin_addr) != 1)
		{
		    perror("inet_pton\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	
		// send string
		if(sendto(sockfd,argv[3],strlen(argv[3]),0,(struct sockaddr *)&dest,addrlen) == -1)
		{
		    perror("sendto\n");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// close the socket
		close(sockfd);
		//On attends une reponse
		struct sockaddr_in my_addr;
		struct sockaddr_in client;
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
		if((sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
		{
		    perror("socket");
		exit(EXIT_FAILURE);
		}
	
		printf("%d\n",INADDR_ANY);
		// init local addr structure and other params
		my_addr.sin_family      = AF_INET;
		my_addr.sin_port        = port_nb;
		my_addr.sin_addr.s_addr = INADDR_ANY;
		addrlen                 = sizeof(struct sockaddr_in);
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
		if(inet_ntop(AF_INET,&client.sin_addr,adr_ip,INET_ADDRSTRLEN)==NULL){
			perror("inet_ntop\n");
			exit(EXIT_FAILURE);		
		}
		printf("Ip source: %s\n",adr_ip);
		printf("Numero de port de l'expediteur: %d\n",client.sin_port);
		// close the socket
		close(sockfd);
	}
	else{
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
	
    struct sockaddr_in my_addr;
    struct sockaddr_in client;

    // check the number of args on command line
    if(argc != 2)
    {
        printf("Usage: %s local_port\n", argv[0]);
	exit(-1);
    }
	
	port_nb=atoi(argv[1]);
    // socket factory
    if((sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        perror("socket");
	exit(EXIT_FAILURE);
    }
	
	// init local addr structure and other params
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = port_nb;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    addrlen                 = sizeof(struct sockaddr_in);
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
	if(inet_ntop(AF_INET,&client.sin_addr,adr_ip,INET_ADDRSTRLEN)==NULL){
		perror("inet_ntop\n");
		exit(EXIT_FAILURE);		
	}
	printf("Ip source: %s\n",adr_ip);
	printf("Numero de port de l'expediteur: %d\n",client.sin_port);
    // close the socket
    close(sockfd);

    return 0;
}
