#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "aes.h"

#define SRV_IP "127.0.0.1"
#define BUFLEN 512
#define PORT 25655

int main(int argc, char **argv)
{
	char* strOpen = "Ouvert";
	char* strClose = "Ferme";
	char* strDone = "Fait";

	char* strComOpen = "Ouvrir";
	char* strComClose = "Fermer";
	char* strComState = "Etat";

	struct sockaddr_in si_other;
	unsigned int s, taille;
	char buf[BUFLEN];

	char commande[BUFLEN];
	char send;

	// Clée de 128 bits UNIQUE QUI FAUT PAS FAIRE CA C'EST PAS BIEN
	uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	// Vecteur d'initialisation
	uint8_t iv[] = {0x00, 0x11, 0x22, 0x13, 0x28, 0xae, 0xd2, 0xa6, 0xa0, 0xf7, 0x15, 0x88, 0x09, 0x78, 0x4f, 0x3c};
	
	uint8_t bufferOut[BUFLEN];
	uint8_t bufferIn[BUFLEN];

	if(argc != 2){
		printf("commande : ./client <ip>\n");
		return 0;
	}
	
	while(strcmp(commande,"quit") != 0){
		printf("Entrez une commande : ");
		scanf("%512s", commande);
		send = 1;

		if(strcmp(commande,"open") == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComOpen);
		else if(strcmp(commande,"close") == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComClose);
		else if(strcmp(commande,"state") == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComState);
		else if(strcmp(commande,"quit") == 0){
			send = 0;
		}
		else{
			printf("commandes : open | close | state | quit\n");
			send = 0;
		}

		if(send){
			//connexion au serveur
			s = socket(AF_INET, SOCK_STREAM, 0);
			if(s == -1 ) return -1;
			bzero(&si_other, sizeof(struct sockaddr_in));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(PORT);
			if( inet_aton(argv[1], &si_other.sin_addr)==0 ) { return -2; }
			if( connect(s, (struct sockaddr*)&si_other, sizeof(struct sockaddr)) < 0 ){
				printf("Connexion échouée\n");
				return -2;
			}

			// Chiffrement
			taille = AES128_CBC_encrypt_buffer(bufferOut, (unsigned char*)buf, taille, key ,iv);
			// Envois
			write(s, bufferOut, taille);
			bzero(buf, BUFLEN);
			
			// Reception
			taille = read(s, bufferIn, BUFLEN);
			// Dechiffrement
			AES128_CBC_decrypt_buffer((unsigned char*)buf, bufferIn, taille, key, iv);

			if(taille < BUFLEN)
				buf[taille] = 0;
			else
				buf[BUFLEN-1] = 0;

			printf("Reponse : %s\n", buf);

			bzero(commande, BUFLEN);
			bzero(buf, BUFLEN);
			bzero(bufferOut, BUFLEN);
			bzero(bufferIn, BUFLEN);

			// Deconnexion
			close(s);
		}
	}	
	return 0;
}
