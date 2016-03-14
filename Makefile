CC           = gcc
CFLAGS       = -Wall -Os
OBJCOPY      = objcopy

# include path to AVR library
INCLUDE_PATH = /usr/lib/avr/include

.SILENT:
.PHONY:  lint clean

serveur.o : serveur.c
	# compiling serveur
	$(CC) $(CFLAGS) -c serveur.c -o serveur.o

client.o : client.c
	# compiling client
	$(CC) $(CFLAGS) -c client.c -o client.o

test.o : test.c
	# compiling test.c
	$(CC) $(CFLAGS) -c test.c -o test.o

aes.o : aes.h aes.c
	# compiling aes.c
	$(CC) $(CFLAGS) -c aes.c -o aes.o

client : aes.o client.o
	# linking object code to binary
	$(CC) $(CFLAGS) aes.o client.o -o client

serveur : aes.o serveur.o
	# linking object code to binary
	$(CC) $(CFLAGS) aes.o serveur.o -o serveur

test.out : aes.o test.o
	# linking object code to binary
	$(CC) $(CFLAGS) aes.o test.o -o test.out

small: test.out
	$(OBJCOPY) -j .text -O ihex test.out rom.hex

clean:
	rm -f *.OBJ *.LST *.o *.gch *.out *.hex *.map
