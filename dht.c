#include "dht.h"


int port_valide(char * port){
	int p = atoi(port);
    if( (p > 65536) || (p <= 0) ){
	    return ERROR;
    }
    return p;
}


int parse_hostname(char * hostname, char * ip){
	struct addrinfo hints, *result, *p;
	struct sockaddr_in6 *h;
    int s;
	
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;         /* Allow IPv6 */
    hints.ai_socktype = SOCK_DGRAM;     /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_UDP;    /* Protocol UDP */

    s = getaddrinfo(hostname, "http", &hints, &result);
    if(s != 0){
        return ERROR;
    }
    
    for(p = result; p != NULL; p = p->ai_next){
 		h = (struct sockaddr_in6 *) p->ai_addr;
 		strcpy(ip, inet_ntoa(h->sin6_addr));	
    }
    
    freeaddrinfo(result);
    return 0;
}
