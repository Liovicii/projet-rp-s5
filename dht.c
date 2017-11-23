#include "dht.h"


int port_valide(char * port){
    int p = atoi(port);
    if( (p > 65536) || (p <= 0) ){
        return ERROR;
    }
    return p;
}


int parse_hostname(char * hostname, char * port, char * ip){
    struct addrinfo hints, *result;
    struct sockaddr_in6 *h;
    int s;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;         /* IPv6 */
    hints.ai_socktype = SOCK_DGRAM;     /* Datagram */
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_UDP;    /* Protocole UDP */

    // on résoud le hostname
    s = getaddrinfo(hostname, port, &hints, &result);
    if(s != 0){
        return ERROR;
    }
    
    // on convertie une adresse binaire en adresse char *
     h = (struct sockaddr_in6 *) result->ai_addr;
     inet_ntop(AF_INET6, &(h->sin6_addr), ip, INET6_ADDRSTRLEN);    

    freeaddrinfo(result);
    return 0;
}


int convert_ipv6(char * arg_ip, char * arg_port, struct sockaddr_in6 * addr){
    int convert;
    char ip[INET6_ADDRSTRLEN];
    convert = inet_pton(AF_INET6, ip, (void*)addr->sin6_addr.s6_addr);
    if(convert <= 0){
        if(convert == 0){
            // IPv6 invalide, on regarde si on a un nom de domaine
            if(parse_hostname(arg_ip, arg_port, ip) == ERROR){
                fprintf(stderr, "Erreur: getaddrinfo unknown arg %s\n", arg_ip);
                return ERROR;
            }
            else{
                inet_pton(AF_INET6,ip,(void*)addr->sin6_addr.s6_addr);
                printf("%s résolu en %s\n", arg_ip, ip);
                return 0;
            }
        }
        else{
            perror("inet_pton");
              return ERROR;     
        }
    }
    return 0;
}



/***** TABLE DHT *****/

DHT * init_dht(char * hash){
	DHT * table = malloc(sizeof(struct hash_cel));
	// on vérifie que le malloc à bien fonctinner
	if(table == NULL){
		fprintf(stderr, "Erreur malloc: allocation echoue\n");
		exit(EXIT_FAILURE);
	}
	// on vérifie que le hash est valide
	if(check_hash(hash) == ERROR){
		fprintf(stderr, "Erreur: hash invalide\n");
		exit(EXIT_FAILURE);
	}
	table->val = hash;
	table->want = NULL;
	table->have = NULL;
	return table;
}

void supp_dht(){

}

void insert_ip_want(){

}

void insert_ip_have(){

}

void insert_hash(){

}

void get_hash(){

}


