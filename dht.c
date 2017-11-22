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
