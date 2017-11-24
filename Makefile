CFLAGS=-Wall -Wextra -Werror
EXEC=server client


# compilation de tous les programmes
all: $(EXEC)

server: dht.o
	gcc server.c $< -o $@ $(CFLAGS)

client: dht.o client.c fctsocket.o
	gcc client.c dht.o fctsocket.o -o $@ $(CFLAGS)

dht.o: dht.c dht.h
	gcc -c $< $(CFLAGS) 

fctsocket.o: fctsocket.h fctsocket.c
	gcc -c fctsocket.h fctsocket.c $(CFLAGS)

# exécution des scripts de tests 
test:
	./test-*.sh

# nettoyage
clean:
	rm -f *.log
	rm -f *.o
