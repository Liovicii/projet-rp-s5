CFLAGS=-Wall -Wextra -Werror -g -lpthread
EXEC=server client
COV=-fprofile-arcs -ftest-coverage
THREAD=-l pthread


# compilation de tous les programmes
all: $(EXEC)

server: dht.o fctsocket.o server.c
	gcc $^ -o $@ $(CFLAGS) $(TREAD)

client: dht.o client.c fctsocket.o
	gcc $^ -o $@ $(CFLAGS)

dht.o: dht.c dht.h
	gcc -c $< $(CFLAGS)

fctsocket.o: fctsocket.h fctsocket.c
	gcc -c $^ $(CFLAGS)

# exécution des scripts de tests 
test:
	./test-*.sh

# nettoyage
clean:
	rm -f test/*.log
	rm -f *.o
