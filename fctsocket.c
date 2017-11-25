#include "fctsocket.h"
#include <sys/types.h>
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

void setip6(char * ip,struct sockaddr_in6 * dest,int sockfd){
	int ret;
	
	if(strcmp("in6addr_any",ip)==0){
		dest->sin6_addr=in6addr_any;
	}
	else{
	
		if((ret=inet_pton(AF_INET6,ip,&dest->sin6_addr)) != 1)
		{
			if (ret == 0){
				fprintf(stderr,"adresse invalide\n");
				exit(EXIT_FAILURE);
			}
			perror("inet_pton\n");
			fermer_socket(sockfd);
			exit(EXIT_FAILURE);
		}
	}
	return;
}

void envoyer_mess6(int sockfd,char * str,struct sockaddr_in6 socket){
	socklen_t addrlen=sizeof(struct sockaddr_in6);
	if(sendto(sockfd,str,strlen(str),0,(struct sockaddr *)&socket,addrlen) == -1)
	{
		perror("sendto\n");
		fermer_socket(sockfd);
		exit(EXIT_FAILURE);
	}

	return;
}

int recevoir_mess6(int sockfd,char * str,int size,struct sockaddr_in6 socket){
	int rec;
	socklen_t addrlen=sizeof(struct sockaddr_in6);
	if((rec=recvfrom(sockfd,str,size,0,(struct sockaddr *)&socket,&addrlen)) == -1)
	{
	  perror("recvfrom");
	  fermer_socket(sockfd);
	  exit(EXIT_FAILURE);
	}

	return rec;
}

void lier_socket6(int sockfd, struct sockaddr_in6 socket){
	socklen_t addrlen=sizeof(struct sockaddr_in6);
	if(bind(sockfd,(struct sockaddr *)&socket,addrlen) == -1)
	{
	  perror("bind");     
	  fermer_socket(sockfd);
	  exit(EXIT_FAILURE);
	}
	return;
}

void creation_chaine(char * type, char * lg,char * ip,char * hash, char * data){
	//On ajoute le type a data
	strncpy(data,type,strlen(type));
	// On concatene la longueur a buf
	strncat(data,lg,strlen(lg));
	//On concatene l'ip au buffer
	//il faudrait que l'ip fasse 46 caracteres
	//printf("Message: %s\n",ip6);
	strncat(data,ip,INET6_ADDRSTRLEN);
	//On concatene le hash au buffer
	strncat(data,hash,strlen(hash));
	return;
}

void remplir_lg(char * ip, char * hash, char * lg){
	//On recupere la taille des deux chaines
	int taille_ip=strlen(ip);
	int taille_hash=strlen(hash);
	//Stockage de la longueur de l'ip dans le premier octet
	lg[0]=(char)taille_ip-'0';
	//Stockage de la longueur du hash dans le deuxieme octet
	lg[1]=(char)taille_hash-'0';
	lg[2]='\0';
	return;
}

void remplir_type(int val_type, char * type){
	snprintf(type,2,"%d",val_type);
	type[1]=('\0');
	return;
}

void extract_string(char * entree,char * sortie, int indice, int t_a_extr){
	strncpy(sortie,entree+indice,t_a_extr);
	sortie[(t_a_extr)]='\0';
	return;
}

int get_length_ip(char * lg){
	return ((int)lg[0]+'0');
}

int get_length_hash(char * lg){
	return ((int)lg[1]+'0');
}

int get_type_from_mess(char * mes){
	char type_m[LENGTH_TYPE+1];
	extract_string(mes,type_m,0,LENGTH_TYPE);
	printf("Type en string: %s\n",type_m);
	return atoi(type_m);
}

void extraire_taille_mess(char * lg_m,char *mes){
	extract_string(mes,lg_m,1,LENGTH_LG);
	return;
}
char * extraire_ip_mess(char * mes){
	char lg_m[LENGTH_LG+1];
	extraire_taille_mess(lg_m,mes);
	printf("extraire_ip: Taille ip= %d\n",get_length_ip(lg_m));
	char * ip=malloc(get_length_ip(lg_m));
	if (ip ==NULL){
		fprintf(stderr,"Erreur extraire_ip_mess\n");
		fprintf(stderr,"On a pas reussi a llouer la memoire\n");
		exit(EXIT_FAILURE);
	}
	extract_string(mes,ip,3,get_length_ip(lg_m));
	return ip;
}

char * extraire_hash_mess(char * mes){
	char lg_m[LENGTH_LG+1];
	extraire_taille_mess(lg_m,mes);
	printf("extraire_ip: Taille hash= %d\n",get_length_hash(lg_m));
	char * hash=malloc(TAILLE_MAX_HASH+1);
	if (hash ==NULL){
		fprintf(stderr,"Erreur extraire_hash_mess\n");
		fprintf(stderr,"On a pas reussi a llouer la memoire\n");
		exit(EXIT_FAILURE);
	}
	extract_string(mes,hash,3+get_length_ip(lg_m),get_length_hash(lg_m));
	return hash;
}
