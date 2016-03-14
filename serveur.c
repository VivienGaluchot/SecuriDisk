#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include "aes.h"

#define BUFLEN 512
#define PORT 25655

int main(void)
{
	char* strOpen = "Ouvert";
	char* strClose = "Ferme";
	char* strDone = "Fait";

	char* strComOpen = "Ouvrir";
	char* strComClose = "Fermer";
	char* strComState = "Etat";
	
	char* temp;

	struct sockaddr_in si_me, si_other;
	unsigned int slen, clisock, s, taille;
	char buf[BUFLEN];
	char open;

	time_t tA;
	time_t tB;

	// Clée de 128 bits UNIQUE QUI FAUT PAS FAIRE CA C'EST PAS BIEN
	uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	// Vecteur d'initialisation
	uint8_t iv[] = {0x00, 0x11, 0x22, 0x13, 0x28, 0xae, 0xd2, 0xa6, 0xa0, 0xf7, 0x15, 0x88, 0x09, 0x78, 0x4f, 0x3c};
	
	uint8_t bufferOut[BUFLEN];
	uint8_t bufferIn[BUFLEN];

	//AES128_CBC_encrypt_buffer(buffer, in, 16, key ,iv);

	if( (s = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) { return -1; }
	taille = 1;
	if( (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &taille, sizeof(int))) == -1 ) { return -1; }
	memset((char*)&si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1 ) { return -2; }

	//initialisation du lecteur
	system("eject -t");
	open = 0;

	while(1){
		listen(s, 5);
		slen = sizeof(si_other);
		if( (clisock=accept(s, (struct sockaddr*)&si_other, &slen)) > 0 ) {
				
			printf("- Connexion de %s:%d\n", inet_ntoa(si_other.sin_addr),ntohs(si_other.sin_port));

			// Réception
			taille = read(clisock, bufferIn, BUFLEN);
			// Déchiffrement
			AES128_CBC_decrypt_buffer((unsigned char*)buf, bufferIn, taille, key, iv);

			if(taille < BUFLEN)
				buf[taille] = 0;
			else
				buf[BUFLEN-1] = 0;

			// Traitement
			printf("Commande : %s\n", buf);
			if(strcmp(buf,strComOpen) == 0){
				system("eject");
				if(open){
					taille = snprintf(buf, BUFLEN, "%s (Deja %s)", strDone, strOpen);
				}
				else{
					taille = snprintf(buf, BUFLEN, "%s", strDone);
					open = 1;
				}
			}
			else if(strcmp(buf,strComClose) == 0){
				system("eject -t");
				if(!open)
					taille = snprintf(buf, BUFLEN, "%s (Deja %s)", strDone, strClose);
				else{
					taille = snprintf(buf, BUFLEN, "%s", strDone);
					open = 0;
				}
			}
			else if(strcmp(buf,strComState) == 0){
				tA = time(0);
				if(open){
					system("eject");
					temp = strOpen;
				}
				else{
					system("eject -t");
					temp = strClose;
				}
				tB = time(0);
				if(tB - tA > 0)
					taille = snprintf(buf, BUFLEN, "%s (le systeme a change)", temp);
				else
					taille = snprintf(buf, BUFLEN, "%s", temp);
			}
			else{
				taille = snprintf(buf, BUFLEN, "Error");
			}

			// Chiffrement
			taille = AES128_CBC_encrypt_buffer(bufferOut, (unsigned char*)buf, taille, key ,iv);
			// Réponse
			write(clisock, bufferOut, taille);

			// Fin de connexion
			close(clisock);			
			bzero(buf, BUFLEN);
			printf("Fait\n");
		}
		else{
			printf("%d-%s\n",errno,strerror(errno));
		}
	}
	close(s);
	return 0;
}