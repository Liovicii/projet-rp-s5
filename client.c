#include "dht.h"
#include "fctsocket.h"



int main(int argc, char **argv)
{
    int sockfd,option_lue;
    char buf[MESS_MAX_SIZE];
	char type[2];
	char length[3];
	char hash[TAILLE_MAX_HASH];
	char ip_hash[TAILLE_MAX_HASH+INET6_ADDRSTRLEN];
	char ip[INET6_ADDRSTRLEN];
	//char * port;
	struct sockaddr_in6 dest;
	struct sockaddr_in6 client;
	struct sockaddr_in6 my_addr;

	//printf("argc %d\n",argc);
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
			default:
				if ( (strcmp("get",argv[3])!=0) && (strcmp("put",argv[3])!=0)){
					fprintf(stderr,"usage: Commande invalide\n");
					fprintf(stderr,"Veuillez entrer une commande valide <get|put>\n");
				}
			}
			fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
			exit(EXIT_FAILURE);
	}
	

	
	if ( (strcmp("get",argv[3])==0)){
		option_lue=GET;
	}
	else{
		option_lue=PUT;
	}

	switch(option_lue){
	case GET:
		
		//On envoie juste un message
		

		// On verifie le nombre d'arguments
		if(argc != 5)
		{
			if(argc == 4){
					fprintf(stderr,"Il doit manquer le hash\n");		
			}
		    fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		    exit(-1);
		}
		
		// On creer le socket ipV6
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);

		my_addr.sin6_family = AF_INET6;
		my_addr.sin6_addr = in6addr_any;
		my_addr.sin6_flowinfo=1;
		my_addr.sin6_scope_id = 0;		
		my_addr.sin6_port = htons(7000);	

		lier_socket6(sockfd,&my_addr);

		

		// on le port de la destination
		//initv6(atoi(argv[2]),&dest);
		if (convert_ipv6(argv[1],argv[2],&dest) == ERROR){
			fprintf(stderr,"je t'aime pas nah\n");
			exit(EXIT_FAILURE);
		}
		dest.sin6_family=AF_INET6;
		dest.sin6_port=htons(atoi(argv[2]));
		// On initialise l'ip de la destinations
		//setip6(argv[1],&dest,sockfd);

		//On met le buffer a 0 avant d'envoyer le message
		memset(buf,'\0',MESS_MAX_SIZE);
		
		// On rempli le type
		remplir_type(GET,type);
		//On remplir la longueur
		remplir_lg("",argv[4],length);
		// On creer la chaine type + lg + ip + hash
		concatener_ip_hash("",argv[4],ip_hash);
		creation_chaine(type,length,ip_hash,buf);
		printf("message que l'on va envoyer: %s\n",buf);

		printf("On va envoyer le hash\n");
		envoyer_mess6(sockfd,buf,dest);
		//fermer_socket(sockfd);
		printf("On a envoyé le hash\n");
		
		// reception de la chaine de caracteres
		printf("On attends de recevoir un message\n");
		// On met le buffer a 0 avant de recevoir le message
		memset(buf,'\0',MESS_MAX_SIZE);
		initv6(sockfd,&client);
		recevoir_mess6(sockfd,buf,MESS_MAX_SIZE,client);
		//printf("On a recu la reponse\n");
		//printf("Message recu: %s\n",buf);
		//printf("Longueur du message: %d\n",rec);

		extract_string(buf,type,0,LENGTH_TYPE);
		extract_string(buf,length,1,LENGTH_LG);
		extract_string(buf,ip,3,get_length_ip(length));
		extract_string(buf,hash,3+get_length_ip(length),get_length_hash(length));

		// hash va contenir les ip qui sont associes au hash sur le serveur
		printf("Ip liées au hash demandé: %s\n",hash);	
		// close the socket
		close(sockfd);
		break;
	case PUT:
		//check nb args == 6
		//On envoie un message
		
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
		
		if (convert_ipv6(argv[1],argv[2],&dest) == ERROR){
			fprintf(stderr,"je t'aime pas nah\n");
			exit(EXIT_FAILURE);
		}
		dest.sin6_family=AF_INET6;
		dest.sin6_port=htons(atoi(argv[2]));
		remplir_lg(argv[5],argv[4],length);
		
		concatener_ip_hash(argv[5],argv[4],ip_hash);
		creation_chaine(type,length,ip_hash,buf);
		
		extract_string(buf,type,0,LENGTH_TYPE);
		extract_string(buf,length,1,LENGTH_LG);
		extract_string(buf,ip,3,get_length_ip(length));
		extract_string(buf,hash,3+get_length_ip(length),get_length_hash(length));

		//printf("Val type: %s\n",type);
		//printf("Val lg: %s\n",length);
		//printf("Val ip mes: %s\n",ip);		
		//printf("Val h mes: %s\n",hash);
		
		//printf("Message complet: %s\n",buf);

		// On creer le socket
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
	
		// On initialise la structure 
		//initv6(atoi(argv[2]),&dest);
		// On initialise l'ip de la structure
		//setip6(argv[1],&dest,sockfd);
		// On envoie le message

		envoyer_mess6(sockfd,buf,dest);
		printf("Me: %s\n",buf);
		// On ferme le socket
		fermer_socket(sockfd);
		break;
	case EXIT:
		if(argc != 5)
		{
		    fprintf(stderr,"usage: %s @dest port_num exit hash \n", argv[0]);
		    exit(-1);
		}
		//On initialise le socket
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
		//On rempli le type du message
		if (convert_ipv6(argv[1],argv[2],&dest) == ERROR){
			fprintf(stderr,"je t'aime pas nah\n");
			exit(EXIT_FAILURE);
		}
		dest.sin6_family=AF_INET6;
		dest.sin6_port=htons(atoi(argv[2]));
		remplir_lg(argv[5],argv[4],length);
		// On initialise la chaine de caractere
		memset(buf,'\0',MESS_MAX_SIZE);

		//On rempli la structure dest
		dest.sin6_family=AF_INET6;
		dest.sin6_port=htons(atoi(argv[2]));
		remplir_lg("",argv[4],length);
		creation_chaine(type,length,argv[4],buf);
		envoyer_mess6(sockfd,buf,dest);
		fermer_socket(sockfd);
		break;
	default:
		fprintf(stderr,"Commande inconnue\n");
		fprintf(stderr,"usage: %s IP PORT COMMANDE HASH [IP]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
	
    
