#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "fctsocket.h"

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

void initv6(int port, struct sockaddr_in6* dest){
	dest->sin6_family = AF_INET6;
	dest->sin6_port   = htons(port);
	dest->sin6_flowinfo=1;
	dest->sin6_scope_id = 0;
	return ;
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

void lier_socket6(int sockfd, struct sockaddr_in6 * socket){
	socklen_t addrlen=sizeof(struct sockaddr_in6);
	if(bind(sockfd,(struct sockaddr *)socket,addrlen) == -1)
	{
	  perror("bind");     
	  fermer_socket(sockfd);
	  exit(EXIT_FAILURE);
	}
	return;
}

void concatener_ip_hash(char *ip,char *hash, char * str){
	strncpy(str,ip,strlen(ip)+1);
	strncat(str,hash,strlen(hash)+1);
	return ;
}

void creation_chaine(char * type, char * lg,char * str, char * data){
	//On ajoute le type a data
	strncpy(data,type,strlen(type));
	// On concatene la longueur a buf
	strncat(data,lg,strlen(lg));
	//On concatene l'ip au buffer
	strncat(data,str,strlen(str));
	return;
}

void remplir_lg(char * ip, char * hash, char * lg){
	//On recupere la taille des deux chaines
	int taille_ip=strlen(ip);
	int taille_hash=strlen(hash);
	int lgt=taille_ip+(taille_hash<<6);
	//printf("taille hash decale: %d\n",taille_hash<<6);
	//printf("taille taille: %d\n",lgt);
	//Stockage de la longueur de l'ip dans le premier octet
	lg[0]=((char)(lgt-((lgt>>6)<<6)))-'0';
	//printf("conversion taille: %c\n",lg[0]);
	//Stockage de la longueur du hash dans le deuxieme octet
	lg[1]=((char)(lgt>>6))-'0';
	//printf("conversion taille: %c\n",lg[1]);
	lg[2]='\0';
	return;
}

void remplir_type(int val_type, char * type){
	snprintf(type,2,"%d",val_type);
	type[1]=('\0');
	return;
}

void extract_string(char * entree,char * sortie, int indice, int t_a_extr){
	//strncpy(sortie,entree+indice,t_a_extr);
	sortie=memcpy(sortie,entree+indice,t_a_extr);
	//printf("Sortie= %s\n",sortie);
	//sortie[t_a_extr];
	sortie[(t_a_extr)]='\0';
	return;
}

int get_length_ip(char * lg){
	int t_ip=(int)lg[0]+'0';
	int t_hash=(int)lg[1]+'0';
	int lgt=t_ip+(t_hash<<6);
	return (lgt-((lgt>>6)<<6));
}

int get_length_hash(char * lg){
	int t_ip=(int)lg[0]+'0';
	int t_hash=(int)lg[1]+'0';
	int lgt=t_ip+(t_hash<<6);
	return ((lgt>>6));
}

int get_type_from_mess(char * mes){
	char type_m[LENGTH_TYPE+1];
	extract_string(mes,type_m,0,LENGTH_TYPE);
	return atoi(type_m);
}

void extraire_taille_mess(char * lg_m,char *mes){
	extract_string(mes,lg_m,1,LENGTH_LG);
	return;
}
char * extraire_ip_mess(char * mes){
	char lg_m[3];
	int lg;
	extraire_taille_mess(lg_m,mes);
	lg=get_length_ip(lg_m);
	char *ip=malloc((lg));
	if (ip ==NULL){
		fprintf(stderr,"Erreur extraire_ip_mess\n");
		fprintf(stderr,"On a pas reussi a llouer la memoire\n");
		exit(EXIT_FAILURE);
	}
	extract_string(mes,ip,3,lg);
	return ip;
}

char * extraire_hash_mess(char * mes){
	char lg_m[4];
	extraire_taille_mess(lg_m,mes);
	char * hash=malloc(TAILLE_MAX_HASH+1);
	if (hash ==NULL){
		fprintf(stderr,"Erreur extraire_hash_mess\n");
		fprintf(stderr,"On a pas reussi a llouer la memoire\n");
		exit(EXIT_FAILURE);
	}
	extract_string(mes,hash,3+get_length_ip(lg_m),get_length_hash(lg_m));
	return hash;
}
