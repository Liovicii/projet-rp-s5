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


int check_hash(char * hash){    
    int lg=strlen(hash);    
    if (lg<65){
        return ERROR;
    }
    if(lg>TAILLE_MAX_HASH){
        return ERROR;
    }
    return 0;
}


/***** TABLE DHT *****/

DHT * init_dht(char * hash){
    DHT * table = malloc(sizeof(DHT));
    if(table == NULL){
        fprintf(stderr, "Erreur: init_dht\n");
        fprintf(stderr, "malloc: allocation echoué\n");
        exit(EXIT_FAILURE);
    }
    strncpy(table->val, hash, strlen(hash));
    table->next = NULL;
    table->want = NULL;
    table->have = NULL;    
    return table;
}



void supp_dht(DHT * table){
    DHT *tmp1_dht = table, *tmp2_dht;
    IP *tmp1_ip, *tmp2_ip;
    
    // traitement du dernier hash de la liste
    while(tmp1_dht->next != NULL){
    
        // suppression liste want
        tmp1_ip = tmp1_dht->want;
        if(tmp1_ip != NULL){
            while(tmp1_ip->next != NULL){
                tmp2_ip = tmp1_ip;
                tmp1_ip = tmp1_ip->next;
                free(tmp2_ip);
            }
            free(tmp1_ip);
        }

        // suppression liste have
        tmp1_ip = tmp1_dht->have;
        if(tmp1_ip != NULL){
            while(tmp1_ip->next != NULL){
                tmp2_ip = tmp1_ip;
                tmp1_ip = tmp1_ip->next;
                free(tmp2_ip);
            }
            free(tmp1_ip);
        }

        // suppression hash_cel
        tmp2_dht = tmp1_dht;
        tmp1_dht = tmp1_dht->next;
        free(tmp2_dht);
    }
    free(tmp1_dht);
}


void affiche_dht(DHT * table){
    DHT * tmp_dht = table;
    IP * tmp_ip;
    // on visite toutes les hash_cel    
    while(tmp_dht != NULL){
        
        // affichage du hash
        printf("HASH: %s\n", tmp_dht->val);
        
        // affichage have
        printf("IP possèdant le hash:\n");
        tmp_ip = tmp_dht->have;
        while(tmp_ip != NULL){
            printf("\tIP: %s\n", tmp_ip->val);
        }
        
        // affichage want
        printf("IP souhaitant le hash:\n");
        tmp_ip = tmp_dht->want;
        while(tmp_ip != NULL){
            printf("\tIP: %s\n", tmp_ip->val);
        }

        // on passe au hash suivant
        tmp_dht = tmp_dht->next;
    }
}


int * get_hash(char * hash){
	DHT * tmp_dht;
	// parcours de la liste des hashs
	
}



/*

void put_hash(char * hash, char * ip){

}


void hash_want_insert(char * hash, char * ip){
    
}


void hash_have_insert(char * hash, char * ip){

}

void hash_want_delete(char * hash, char * ip){

}

void hash_have_delete(char * hash, char * ip){

}


*/
