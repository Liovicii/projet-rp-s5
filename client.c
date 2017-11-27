#include "dht.h"
#include "fctsocket.h"



int main(int argc, char **argv)
{
    int sockfd;
    char buf[MESS_MAX_SIZE];
	char type[2];
	char length[3];
	
	if(argc<=4){
		switch(argc){
			case 1:
				fprintf(stderr,"Il n'y a pas d'arguments\n");
				break;
			case 2:
				fprintf(stderr,"Il doit un port, une commande, un hash\n");
				break;
			case 3:
				fprintf(stderr,"Il doit manquer une commande et un hash\n");
				break;
			case 4:
				fprintf(stderr,"Veuillez entrer une commande (get/set)\n");
		}
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	if(strcmp(argv[3],"get")==0){
		
		//On envoie juste un message
		struct sockaddr_in6 dest;

		// On verifie le nombre d'arguments
		if(argc != 5)
		{
			if(argc == 4){
					fprintf(stderr,"Il doit manquer le hash\n");		
			}
		    fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		    exit(-1);
		}
		struct sockaddr_in6 my_addr;
		// On creer le socket ipV6
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
		
		my_addr=initv6(7000);
		setip6("in6addr_any",&my_addr,sockfd);
		
		memset(buf,'\0',MESS_MAX_SIZE);
		//On lie la structure au socket
		lier_socket6(sockfd, my_addr);

		// on initialise la structure
		dest=initv6(atoi(argv[2]));
		
		// On initialise l'ip du socket
		setip6(argv[1],&dest,sockfd);
		// On envoie le message
		remplir_type(GET,type);
		remplir_lg("",argv[4],length);
		creation_chaine(type,length,concatener_ip_hash("",argv[4]),buf);
		printf("message que l'on va envoyer: %s\n",buf);
		
		char * hashe;
		hashe = extraire_hash_mess(buf);
		printf("hash= %s\n",hashe);
		printf("hash= %s\n",buf+4);
		printf("On va envoyer le hash\n");
		envoyer_mess6(sockfd,buf,dest);
		//fermer_socket(sockfd);
		printf("On a envoyé le hash\n");
		
		//On attends une reponse
		
		struct sockaddr_in6 client;
		// On initilise le socket
//		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);

		
		// reception de la chaine de caracteres
		printf("On attends de recvoir un message\n");
		int rec=recevoir_mess6(sockfd,buf,MESS_MAX_SIZE,client);
		printf("On a recu la reponse\n");
		// print the received char
		printf("Message recu: %s\n",buf);
		printf("Longueur du message: %d\n",rec);
	
		char adr_ip[INET6_ADDRSTRLEN];
		if(inet_ntop(AF_INET6,&client.sin6_addr,adr_ip,INET6_ADDRSTRLEN)==NULL){
			perror("inet_ntop\n");
			exit(EXIT_FAILURE);		
		}
		printf("Ip source: %s\n",adr_ip);
		printf("Numero de port de l'expediteur: %d\n",client.sin6_port);
		

		char type_m[LENGTH_TYPE+1];
		char lg_m[LENGTH_LG+1];
		extract_string(buf,type_m,0,LENGTH_TYPE);
		extract_string(buf,lg_m,1,LENGTH_LG);

		char hash[TAILLE_MAX_HASH];
		char recup[get_length_ip(length)+1];
		extract_string(buf,recup,3,get_length_ip(lg_m));
		extract_string(buf,hash,3+get_length_ip(lg_m),get_length_hash(lg_m));

		printf("Val type: %s\n",type_m);
		printf("Val lg: %s\n",lg_m);
		printf("Val ip mes: %s\n",recup);		
		printf("Val h mes: %s\n",hash);	
		// close the socket
		close(sockfd);
	}
	else if(strcmp(argv[3],"put")==0){
		//check nb args == 6
		//On envoie un message
		struct sockaddr_in6 dest;

		// check the number of args on command line
		if(argc != 6)
		{
			switch(argc){
				case 4:
					fprintf(stderr,"Il doit manquer le hash et l'ip\n");
					break;
				case 5:
					fprintf(stderr,
					"Il doit manquer l'ip qu'on va associer au hash\n");		
			}
		    printf("USAGE: %s @dest port_num put hash ip\n", argv[0]);
		    exit(-1);
		}
				
		remplir_type(PUT,type);
		memset(buf,'\0',MESS_MAX_SIZE);

			
		//On regarde si le hash est valide
		if(check_hash(argv[4])==-1){
			fprintf(stderr,"usage: le hash n'est pas assez long\n");
			exit(EXIT_FAILURE);
		}
		
		remplir_lg(argv[5],argv[4],length);
		
		creation_chaine(type,length,concatener_ip_hash(argv[5],argv[4]),buf);
		

		
		char type_m[LENGTH_TYPE+1];
		char lg_m[LENGTH_LG+1];
		extract_string(buf,type_m,0,LENGTH_TYPE);
		extract_string(buf,lg_m,1,LENGTH_LG);

		char hash[TAILLE_MAX_HASH];
		char recup[get_length_ip(length)+1];		
		extract_string(buf,recup,3,get_length_ip(lg_m));
		extract_string(buf,hash,3+get_length_ip(lg_m),get_length_hash(lg_m));

		printf("Val type: %s\n",type_m);
		printf("Val lg: %s\n",lg_m);
		printf("Val ip mes: %s\n",recup);		
		printf("Val h mes: %s\n",hash);
		
		printf("Message complet: %s\n",buf);

		// On creer le socket
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);	
		// On initialise la structure 
		dest=initv6(atoi(argv[2]));
		// On initialise l'ip de la structure
		setip6(argv[1],&dest,sockfd);
		// On envoie le message
		envoyer_mess6(sockfd,buf,dest);
		// On ferme le socket
		fermer_socket(sockfd);
	}
	else{
		fprintf(stderr,"Commande inconnue\n");
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
	
    
