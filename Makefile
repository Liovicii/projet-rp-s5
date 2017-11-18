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
	# à completer

# nettoyage
clean:
	rm -f *.o
