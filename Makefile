CC           = gcc
CFLAGS       = -Wall

all: serveur client

serveur.o : serveur.c
	# compiling serveur
	$(CC) $(CFLAGS) -c serveur.c -o serveur.o

client.o : client.c
	# compiling client
	$(CC) $(CFLAGS) -c client.c -o client.o

aes.o : aes.h aes.c
	# compiling aes.c
	$(CC) $(CFLAGS) -c aes.c -o aes.o

sha256.o : sha256.h sha256.c
	# compiling sha256.c
	$(CC) $(CFLAGS) -c sha256.c -o sha256.o

client : sha256.o  aes.o client.o
	# linking object code to binary
	$(CC) $(CFLAGS) sha256.o aes.o client.o -o client

serveur : sha256.o aes.o serveur.o
	# linking object code to binary
	$(CC) $(CFLAGS) sha256.o aes.o serveur.o -o serveur

clean:
	rm -f *.OBJ *.LST *.o *.gch *.out *.hex *.map