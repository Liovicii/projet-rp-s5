CFLAGS=-Wall -Wextra -Werror
EXEC=server client


# compilation de tous les programmes
all: $(EXEC)

server: dht.o
	gcc server.c $< -o $@ $(CFLAGS)

client: client.c dht.o
	gcc client.c -o $@ $(CFLAGS)

dht.o: dht.c dht.h
	gcc -c $< $(CFLAGS) 

# exécution des scripts de tests 
test:
	./test-*.sh

# nettoyage
clean:
	rm -f *.log
	rm -f *.o
