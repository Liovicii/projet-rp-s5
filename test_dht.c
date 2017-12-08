#include "dht.h"

int main(){
	DHT * table=init_dht("bwaaaaaaa");
	affiche_dht(table);
	if (put_hash("wazaaaa","::1",table)==-1){
		fprintf(stderr,"Erreur put_hash");
		exit(EXIT_FAILURE);
	}
	if (put_hash("wazaaaa","::",table)==-1){
		fprintf(stderr,"Erreur put_hash");
		exit(EXIT_FAILURE);
	}
	if (put_hash("wazaaaa","::2",table)==-1){
		fprintf(stderr,"Erreur put_hash");
		exit(EXIT_FAILURE);
	}
	affiche_dht(table);
	delete_ip("wazaaaa","::",table,HAVE);
	affiche_dht(table);
	return EXIT_SUCCESS;
}
