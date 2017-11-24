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



char * get_hash(char * hash, DHT * table){

    DHT * tmp_dht = table;
    IP * tmp_ip;
    char * ips = malloc(INET6_ADDRSTRLEN*MAX_IPS);

    // vérification des arguments
    if(hash == NULL){
        fprintf(stderr, "Erreur: get_hash\n");
        fprintf(stderr, "hash value is %s\n", hash);
        return NULL;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: get_hash\n");
        fprintf(stderr, "table is NULL\n");
        return NULL;
    }
    
    // on cherche le hash
    while((strncmp(hash,tmp_dht->val,strlen(hash)) != 0)){
        tmp_dht = tmp_dht->next;
        // si on sort de la liste, on sort de la fonction
        if(tmp_dht == NULL){
            fprintf(stderr, "Erreur: get_hash\n");
            fprintf(stderr, "hash %s not found\n", hash);
            return NULL;
        }
    }
    
    // on crée la chaine qui contiendra la liste des IP qui possède le hash
    tmp_ip = tmp_dht->have;
    if(tmp_ip == NULL){
        fprintf(stderr, "Erreur: get_hash\n");
        fprintf(stderr, "nobody has hash %s :(\n", hash);
        return NULL;
    }
    while(tmp_ip != NULL){
        strncat(ips, " ", 1);
        strncat(ips, tmp_ip->val, strlen(tmp_ip->val));
        if(ips == NULL){
            fprintf(stderr, "Erreur: get_hash\n");
            fprintf(stderr, "strncat: ips has value %s\n", ips);
        }
    }

    return ips;
}




/*
 * \fn int insert_hash (char * hash, DHT * table)
 * \brief Insert un hash dans une table de hash
 *
 * \param hash un hash au format string
 * \param table pointeur vers une table
 * \return 0 si tout se passe bien, -1 si erreur, NTD(2) si rien à faire
 */
int insert_hash(char * hash, DHT * table){

    DHT *tmp_dht = table, *new;

    // vérification des arguments
    if(hash == NULL){
        fprintf(stderr, "Erreur: insert_hash\n");
        fprintf(stderr, "hash value is %s\n", hash);
        return ERROR;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: insert_hash\n");
        fprintf(stderr, "table is NULL\n");
        return ERROR;
    }

    // parcours de la table
    while(tmp_dht->next != NULL){

        // on vérifie que le hash n'existe pas déjà
        if(strncmp(tmp_dht->val, hash, strlen(hash)) == 0){
            fprintf(stderr, "Erreur: insert_hash\n");
            fprintf(stderr, "hash %s has already an entry !\n", hash);
            return NTD;
        }
        tmp_dht = tmp_dht->next;
    }
    
    // création du hash_cel
    new = malloc(sizeof(DHT));
    if(new == NULL){
        fprintf(stderr, "Erreur: insert_hash\n");
        fprintf(stderr, "malloc failed to init hash_cel\n");
        return ERROR;
    }
    
    strncpy(new->val, hash, strlen(hash));
    if(new->val == NULL){
        fprintf(stderr, "Erreur: insert_hash\n");
        fprintf(stderr, "failed to copy hash into new hash_cel\n");
        return ERROR;
    }
    new->next = NULL;
    new->want = NULL;
    new->have = NULL;
    
    // on attache l'élément new en fin de chaine
    tmp_dht->next = new;

    // tout s'est bien passé
    return 0;
}




/*
 * \fn int insert_ip (IP * liste, char * ip)
 * \brief Insert une ip dans une liste d'IP
 *
 * \param liste une liste chaine d'IP
 * \param ip une adresse IP
 * \return 0 si tout se passe bien, -1 si erreur, NTD(2) si rien à faire
 */
int insert_ip(IP * liste, char * ip){
	
	IP * tmp_ip = liste, *new;

	// verification des arguments
	if(liste == NULL){
		fprintf(stderr, "Erreur: insert_ip\n");
		fprintf(stderr, "IP liste is NULL\n");
		return ERROR;
	}
	if(ip == NULL){
		fprintf(stderr, "Erreur: insert_ip\n");
		fprintf(stderr, "IP [%s] is NULL\n", ip);
		return ERROR;
	}

	// insertion IP
	while(tmp_ip->next != NULL){
		if(strncmp(tmp_ip->val, ip, strlen(ip)) == 0){
			fprintf(stderr, "Erreur: insert_ip\n");
			fprintf(stderr, "IP %s already in list\n", ip);
			return NTD;
		}
		tmp_ip = tmp_ip->next;
	}

	// creation de l'ip_cel
	new = malloc(sizeof(IP));
	if(new == NULL){
		fprintf(stderr, "Erreur: insert_ip\n");
		fprintf(stderr, "malloc failed on new ip_cel\n");
		return ERROR;
	}

    strncpy(new->val, ip, strlen(ip));
    if(new->val == NULL){
        fprintf(stderr, "Erreur: insert_ip\n");
        fprintf(stderr, "failed to copy ip into new ip_cel\n");
        return ERROR;
    }
    new->next = NULL;
    
    // on attache l'élément new en fin de chaine
    tmp_ip->next = new;
	
	// tout s'est bien passé
	return 0;
}



int put_hash(char * hash, char * ip, DHT * table){

    DHT * tmp_dht = table;

    // recherche du hash
    while((strncmp(tmp_dht->val,hash,strlen(hash)) != 0) && (tmp_dht!=NULL)){
        tmp_dht = tmp_dht->next;
    }

    // on regarde ce qu'on doit faire
    if(tmp_dht == NULL){
        // on a pas trouve le hash donc on l'insere
        if(insert_hash(hash, table) == ERROR){
            fprintf(stderr, "Erreur: put_hash\n");
            fprintf(stderr, "Insertion hash failed\n");
            return ERROR;
        }
        tmp_dht = tmp_dht->next;
    }
    // le hash existe ou a été créé, on insert l'IP
    if(insert_ip(tmp_dht->have, ip) == ERROR){
        fprintf(stderr, "Erreur: put_hash\n");
        fprintf(stderr, "Insertion IP failed\n");
        return ERROR;
    }

    // houf, tout s'est bien passé
    return 0;
}


/*

void hash_want_delete(char * hash, char * ip){

}

void hash_have_delete(char * hash, char * ip){

}


*/
