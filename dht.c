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
                // association à l'adresse réseau de l'ip du hostname résolu
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
        fprintf(stderr, "Erreur: init_dht");
        fprintf(stderr, "\tmalloc: allocation echoué\n");
        exit(EXIT_FAILURE);
    }
    strncpy(table->val, hash, strlen(hash));
       if(table->val == NULL){
        fprintf(stderr, "Erreur: init_dht");
        fprintf(stderr,"\tstrncpy failed to copie %s into %s\n",table->val,hash);
    }
    table->next = NULL;
    table->want = NULL;
    table->have = NULL;    
    return table;
}



/*
 * \fn void delete_ip_list (IP * liste)
 * \brief supprime une liste d'IP
 *
 * \param liste une liste d'adresse IP
 */
void delete_ip_list(IP * liste){
    IP *tmp1_ip = liste, *tmp2_ip;
    if(liste == NULL){
        // la liste est vide pas besoin de la supprimer
        return;
    }
    while(tmp1_ip != NULL){
        tmp2_ip = tmp1_ip;
        tmp1_ip = tmp1_ip->next;
        free(tmp2_ip);
    }
}




void supp_dht(DHT * table){
    DHT *tmp1_dht = table, *tmp2_dht;
    
    // traitement du dernier hash de la liste
    while(tmp1_dht != NULL){
    
        // suppression liste want
        delete_ip_list(tmp1_dht->want);

        // suppression liste have
        delete_ip_list(tmp1_dht->have);

        // suppression hash_cel
        tmp2_dht = tmp1_dht;
        tmp1_dht = tmp1_dht->next;
        free(tmp2_dht);
    }
}



void affiche_dht(DHT * table){
    DHT * tmp_dht = table;
    IP * tmp_ip;
    printf("\n\nAffichage de la table:\n");
    // on visite toutes les hash_cel    
    while(tmp_dht != NULL){
        
        // affichage du hash
        printf("HASH: %s\n", tmp_dht->val);
        
        // affichage have
        printf("  IP have: ");
        tmp_ip = tmp_dht->have;
        while(tmp_ip != NULL){
            printf("\t %s ", tmp_ip->val);
            tmp_ip = tmp_ip->next;
        }
        
        // affichage want
        printf("\n  IP want: ");
        tmp_ip = tmp_dht->want;
        while(tmp_ip != NULL){
            printf("\t %s ", tmp_ip->val);
            tmp_ip = tmp_ip->next;
        }

        // on passe au hash suivant
        tmp_dht = tmp_dht->next;
        printf("\n");
    }
    printf("\n");
}




char * get_hash(char * hash, DHT * table){

    DHT * tmp_dht = table;
    IP * tmp_ip;
    char * ips = malloc(INET6_ADDRSTRLEN*MAX_IPS);
    int c=0;

    // vérification des arguments
    if(hash == NULL){
        fprintf(stderr, "Erreur: get_hash");
        fprintf(stderr, "\thash value is %s\n", hash);
        return NULL;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: get_hash");
        fprintf(stderr, "\ttable is NULL\n");
        return NULL;
    }
    
    // on cherche le hash
    while((strncmp(hash,tmp_dht->val,strlen(hash)) != 0)){
        tmp_dht = tmp_dht->next;
        // si on sort de la liste, on sort de la fonction
        if(tmp_dht == NULL){
            fprintf(stderr, "Erreur: get_hash");
            fprintf(stderr, "\nhash %s not found\n", hash);
            return NULL;
        }
    }
    
    // on crée la chaine qui contiendra la liste des IP qui possède le hash
    tmp_ip = tmp_dht->have;
    if(tmp_ip == NULL){
        fprintf(stderr, "Erreur: get_hash");
        fprintf(stderr, "\tnobody has hash %s :(\n", hash);
        return NULL;
    }
    // on inserera au maximum 10 adresses IP
    while((tmp_ip != NULL) && (c != 10)){
        strncat(ips, " ", 1);
        strncat(ips, tmp_ip->val, strlen(tmp_ip->val));
        if(ips == NULL){
            fprintf(stderr, "Erreur: get_hash");
            fprintf(stderr, "\tstrncat: ips has value %s\n", ips);
        }
        tmp_ip = tmp_ip->next;
        c++;
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
        fprintf(stderr, "Erreur: insert_hash");
        fprintf(stderr, "\thash value is %s\n", hash);
        return ERROR;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: insert_hash");
        fprintf(stderr, "\ttable is NULL\n");
        return ERROR;
    }

    // parcours de la table
    while(tmp_dht->next != NULL){

        // on vérifie que le hash n'existe pas déjà
        if(strncmp(tmp_dht->val, hash, strlen(hash)) == 0){
            printf("hash %s has already an entry !\n", hash);
            return NTD;
        }
        tmp_dht = tmp_dht->next;
    }
   
    // création du hash_cel
    new = malloc(sizeof(DHT));
    if(new == NULL){
        fprintf(stderr, "Erreur: insert_hash");
        fprintf(stderr, "\tmalloc failed to init hash_cel\n");
        return ERROR;
    }
    
    strncpy(new->val, hash, strlen(hash));
    if(new->val == NULL){
        fprintf(stderr, "Erreur: insert_hash");
        fprintf(stderr, "\tfailed to copy hash into new hash_cel\n");
        return ERROR;
    }
    
    new->val[strlen(hash)]='\0';
    new->next = NULL;
    new->want = NULL;
    new->have = NULL;
    
    // on attache l'élément new en fin de chaine
    tmp_dht->next = new;
	printf("Valeure inserée %s\n",tmp_dht->next->val);
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
int insert_ip(DHT * hash, char * ip, int liste){
   
    IP * tmp_ip, *new;

    // verification des arguments
    if(ip == NULL){
        fprintf(stderr, "Erreur: insert_ip");
        fprintf(stderr, "\tIP is NULL\n");
        return ERROR;
    }
    if(hash == NULL){
        fprintf(stderr, "Erreur: insert_ip");
        fprintf(stderr, "\thash is NULL\n");
        return ERROR;
    }
    if(liste == WANT) tmp_ip = hash->want;    
    else if(liste == HAVE) tmp_ip = hash->have;
    else{
        fprintf(stderr, "Erreur: insert_ip");
        fprintf(stderr, "\tUnknown liste type: %d\n", liste);
        return ERROR;
    }

    // on regarde si la liste existe déjà
    if(tmp_ip != NULL){
        // on va au bout de la liste
        while(tmp_ip->next != NULL){
            if(strncmp(tmp_ip->val, ip, strlen(ip)) == 0){
                printf("\tIP %s already in list\n", ip);
                return NTD;
            }
            tmp_ip = tmp_ip->next;
           }
    }

    // creation de l'ip_cel
       new = malloc(sizeof(IP));
       if(new == NULL){
           fprintf(stderr, "Erreur: insert_ip");
        fprintf(stderr, "\tmalloc failed on new ip_cel\n");
        return ERROR;
    }

    // remplissage de l'ip_cel
    strncpy(new->val, ip, strlen(ip));
    if(new->val == NULL){
        fprintf(stderr, "Erreur: insert_ip");
        fprintf(stderr, "\tfailed to copy ip into new ip_cel\n");
        return ERROR;
    }
    new->next = NULL;
    
    // on attache l'élément new en fin de chaine ou au hash
    if(tmp_ip != NULL) tmp_ip->next = new;
    else if(liste == HAVE) hash->have = new;
    else if(liste == WANT) hash->want = new;

    // tout s'est bien passé
    return 0;
}



int put_hash(char * hash, char * ip, DHT ** table){

    DHT * tmp_dht;

    // check args.
    if(hash == NULL){
        fprintf(stderr, "Erreur: put_hash");
        fprintf(stderr, "\thash argument is NULL\n");
        return ERROR;
    }
    if(ip == NULL){
        fprintf(stderr, "Erreur: put_hash");
        fprintf(stderr, "\tip argument is NULL\n");
        return ERROR;
    }
    if(*table == NULL){
		// il faut créer la table
		printf("=> table inexistante. Initialisation.\n");
		*table = init_dht(hash);
	}

	tmp_dht = *table;

    // recherche du hash
    while((tmp_dht!=NULL) && (strncmp(tmp_dht->val,hash,strlen(hash)) != 0)){
        tmp_dht = tmp_dht->next;
    }

    // on regarde ce qu'on doit faire
    if(tmp_dht == NULL){
        // on a pas trouve le hash donc on l'insere
        printf("=> hash inexistant: insertion hash...\n");
        if(insert_hash(hash, *table) == ERROR){
            fprintf(stderr, "Erreur: put_hash");
            fprintf(stderr, "\tInsertion hash failed\n");
            return ERROR;
        }
         // tmp_dht = à l'adresse du nouveau hash
        tmp_dht = *table;
        while(tmp_dht->next != NULL) tmp_dht = tmp_dht->next;
    }

    // le hash existe ou a été créé, on insert l'IP
    if(tmp_dht->have != NULL){
        if(insert_ip(tmp_dht, ip, HAVE) == ERROR){
            fprintf(stderr, "Erreur: put_hash");
            fprintf(stderr, "\tInsertion IP failed\n");
            return ERROR;
        }
    }
    else{
        // creation manuelle car la liste n'existe pas encore
        tmp_dht->have = malloc(sizeof(IP));
        if(tmp_dht->have == NULL){
            fprintf(stderr, "Erreur: put_hash");
            fprintf(stderr, "\tmalloc: creation of have list failed\n");
            return ERROR;
        }
        strncpy(tmp_dht->have->val, ip, strlen(ip));
        tmp_dht->have->next = NULL;
    }

    // houf, tout s'est bien passé
    return 0;
}





void delete_hash(char * hash, DHT ** table){
    
    DHT * tmp_dht = *table, *old = NULL;
    
    // verif. args.
    if(hash == NULL){
        fprintf(stderr, "Erreur: delete_hash");
        fprintf(stderr, "\targument hash is NULL\n");
        return;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: delete_hash");
        fprintf(stderr, "\targument table is NULL\n");
        return;
    }

    // on cherche le hash
    while((tmp_dht!=NULL)&&(strncmp(tmp_dht->val,hash,strlen(hash))!=0)){
        old = tmp_dht;
        tmp_dht = tmp_dht->next;
    }
    
    if(tmp_dht == NULL){
        fprintf(stderr, "Erreur: delete_hash");
        fprintf(stderr, "\tHash %s not in table or already deleted\n", hash);
        return;
    }
    
    // suppression du hash_cel
    if(old == NULL){
        // cas particulier de la suppression du 1er élément
        *table = (*table)->next;
    }
    else{
        old->next = tmp_dht->next;
    }
    delete_ip_list(tmp_dht->want);
    delete_ip_list(tmp_dht->have);
    free(tmp_dht);
}




void delete_ip(char * hash, char * ip, DHT * table, int liste){
    
    DHT * tmp_dht = table;
    IP *tmp_ip, *old = NULL;

    // verif. args.
    if(hash == NULL){
        fprintf(stderr, "Erreur: delete_ip");
        fprintf(stderr, "\targument hash %s is NULL\n", hash);
        return;
    }
    if(ip == NULL){
        fprintf(stderr, "Erreur: delete_ip");
        fprintf(stderr, "\targument ip %s NULL\n", ip);
        return;
    }
    if(table == NULL){
        fprintf(stderr, "Erreur: delete_ip");
        fprintf(stderr, "\targument table is NULL\n");
        return;
    }

    // on recherche le hash
    while((tmp_dht!=NULL)&&(strncmp(tmp_dht->val,hash,strlen(hash))!=0)){
        tmp_dht = tmp_dht->next;
    }
    // le hash est introuvable
    if(tmp_dht == NULL){
        fprintf(stderr, "Erreur: delete_ip");
        fprintf(stderr, "\thash %s not found or already deleted\n", hash);
        return;
    }

    // on regarde dans quelle liste d'IP on doit supprimer l'ip
    if(liste == WANT){
        tmp_ip = tmp_dht->want;
    }
    else if(liste == HAVE){
        tmp_ip = tmp_dht->have;
    }
    else{
        // liste inconnue
        fprintf(stderr, "Erreur delete_ip");
        fprintf(stderr, "\thash_cel has no list called %d\n", liste);
        return;
    }
    
    // recherche de l'IP
    int chercher=1;
    while((tmp_ip!=NULL)&& chercher){
    	if(strlen(tmp_ip->val)==strlen(ip)){
    		if (strncmp(tmp_ip->val,ip,strlen(ip))!=0){
				old = tmp_ip;
        		tmp_ip = tmp_ip->next;    		
    		}
    		else{
    			chercher=0;
    		}
    	}
    	else{
    		old = tmp_ip;
        	tmp_ip = tmp_ip->next;
        } 
    }

    // l'ip est introuvable
    if(tmp_ip == NULL){
        fprintf(stderr, "Erreur: delete_ip");
        fprintf(stderr, "\tIP %s not found or already deleted\n", ip);
        return;
    }

    // suppression de l'IP
    // si on est au premier élément de la liste
    if(old == NULL){
        if(liste == HAVE) tmp_dht->have = tmp_ip->next;
        if(liste == WANT) tmp_dht->want = tmp_ip->next;
    }
    else old->next = tmp_ip->next;
    free(tmp_ip); 
}
