CFLAGS=-Wall -Wextra -Werror
EXEC=server


# compilation de tous les programmes
all: $(EXEC)

server: dht.o
	gcc server.c $< -o $@ $(CFLAGS)

dht.o: dht.c dht.h
	gcc -c $< $(CFLAGS) 

# exécution des scripts de tests 
test:
	./test-*.sh

# nettoyage
clean:
	rm -f *.log
	rm -f *.o
