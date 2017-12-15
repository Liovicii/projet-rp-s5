#include "dht.h"
#include "fctsocket.h"
#define PORT_CLIENT	15898

/*
 * \fn void usage (char * arg)
 * \brief Fonction usage; affiche le fonctionnement du programme
 * \param arg Le nom du programme (généralement argv[0]).
 */
void usage(char * arg){
	fprintf(stderr,"usage: %s IP PORT <get/exit> HASH [IP]\n",arg);
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv)
{
    int sockfd,option_lue;
    int ret;
    int max_sd;
    char buf[MESS_MAX_SIZE];
	char type[2];
	char length[3];
	char hash[TAILLE_MAX_HASH];
	char ip_hash[TAILLE_MAX_HASH+INET6_ADDRSTRLEN];
	char ip[INET6_ADDRSTRLEN];
	struct sockaddr_in6 dest;
	struct sockaddr_in6 client;
	struct sockaddr_in6 my_addr;
	struct timeval waitTh;
	fd_set read_sds;
	//printf("argc %d\n",argc);

		switch(argc){
			case 5:
				if ( (strcmp("get",argv[3])==0)){
					option_lue=GET;
					//On regarde si le hash est valide
					if(check_hash(argv[4])==-1){
						fprintf(stderr,"Erreur: le hash n'est pas assez long\n");
						usage(argv[0]);
					}
				}
				else if ( (strcmp("exit",argv[3])==0)){
					option_lue=EXIT;
				}
				else{
					usage(argv[0]);
				}
				break;
			case 6:
				if ( (strcmp("put",argv[3])==0)){
					option_lue=PUT;
					//On regarde si le hash est valide
					if(check_hash(argv[4])==-1){
						fprintf(stderr,"Erreur: le hash n'est pas assez long\n");
						usage(argv[0]);
					}
				}
				else{
					usage(argv[0]);
				}
				break;
			default:
				usage(argv[0]);
		}
	
	// verification du port
	if(port_valide(atoi(argv[2]))	== ERROR){
		fprintf(stderr, "Erreur: %d port invalide", atoi(argv[2]));
		usage(argv[0]);
	}
	
	switch(option_lue){
	case GET:
		
		//On envoie juste un message

		// On creer le socket ipV6
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);

		my_addr.sin6_family = AF_INET6;
		my_addr.sin6_addr = in6addr_any;
		my_addr.sin6_flowinfo=1;
		my_addr.sin6_scope_id = 0;		
		my_addr.sin6_port = htons(PORT_CLIENT);	

		lier_socket6(sockfd,&my_addr);

		//On rempli la strucutre destination
		if (convert_ipv6(argv[1],atoi(argv[2]),&dest) == ERROR){
			fprintf(stderr,"Erreur: %s IP invalide\n", argv[1]);
			usage(argv[0]);
		}

		//On met le buffer a 0 avant d'envoyer le message
		memset(buf,'\0',MESS_MAX_SIZE);
		
		// On rempli le type
		remplir_type(GET,type);
		//On remplir la longueur
		remplir_lg("",argv[4],length);
		// On creer la chaine type + lg + ip + hash
		concatener_ip_hash("",argv[4],ip_hash);
		creation_chaine(type,length,ip_hash,buf);

		envoyer_mess6(sockfd,buf,dest);
		
		// reception de la chaine de caracteres
		// On met le buffer a 0 avant de recevoir le message
		memset(buf,'\0',MESS_MAX_SIZE);
		initv6(sockfd,&client);
		
		max_sd=sockfd+1;
		waitTh.tv_sec 	= 5;
		waitTh.tv_usec 	= 50;
		FD_ZERO(&read_sds);
		FD_SET(sockfd, &read_sds);
		ret= select(max_sd,&read_sds,NULL, NULL, &waitTh);
		if (ret <0){
			perror("select");
			exit(EXIT_FAILURE);
		}
		else if (FD_ISSET(sockfd,&read_sds)){
			recevoir_mess6(sockfd,buf,MESS_MAX_SIZE,client);
			extract_string(buf,type,0,LENGTH_TYPE);
			extract_string(buf,length,1,LENGTH_LG);
			extract_string(buf,ip,3,get_length_ip(length));
			extract_string(buf,hash,3+get_length_ip(length),get_length_hash(length));

			// hash va contenir les ip qui sont associes au hash sur le serveur
			printf("Ip liées au hash demandé: %s\n",hash);	
		}
		else{
			printf("On a pas recu de reponse de la part du serveur\n");
			printf("Veuillez vous assurer que ce serveur est actif\n");
		}
			// close the socket
		close(sockfd);
		break;
	case PUT:
		//check nb args == 6
		//On envoie un message
		
		remplir_type(PUT,type);
		memset(buf,'\0',MESS_MAX_SIZE);
		
		if (convert_ipv6(argv[1],atoi(argv[2]),&dest) == ERROR){
			fprintf(stderr,"Erreur conversion ip\n");
			exit(EXIT_FAILURE);
		}
		
		remplir_lg(argv[5],argv[4],length);
		
		concatener_ip_hash(argv[5],argv[4],ip_hash);
		creation_chaine(type,length,ip_hash,buf);
		
		extract_string(buf,type,0,LENGTH_TYPE);
		extract_string(buf,length,1,LENGTH_LG);
		extract_string(buf,ip,3,get_length_ip(length));
		extract_string(buf,hash,3+get_length_ip(length),get_length_hash(length));

		// On creer le socket
		sockfd=creer_socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
	
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
		//On rempli la structure dest
		if (convert_ipv6(argv[1],atoi(argv[2]),&dest) == ERROR){
			fprintf(stderr,"je t'aime pas nah\n");
			exit(EXIT_FAILURE);
		}
		remplir_type(EXIT,type);
		// On initialise la chaine de caractere
		memset(buf,'\0',MESS_MAX_SIZE);

		//On creer le message
		remplir_lg("",argv[4],length);
		creation_chaine(type,length,argv[4],buf);
		// On envoi le message
		envoyer_mess6(sockfd,buf,dest);
		fermer_socket(sockfd);
		break;
	default:
		fprintf(stderr,"Commande inconnue\n");
		usage(argv[0]);
	}
	
	exit(EXIT_SUCCESS);
}
	
    
