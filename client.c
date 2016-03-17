#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "aes.h"
#include "sha256.h"

#define BUFLEN 512
#define PORT 25655

#define CLI_AUTH 1
#define SERV_AUTH 2
#define CLI_MES 3
#define	SER_MES 4

struct User {
	char id[BUFLEN];
	char mdp[BUFLEN];
};

int main(int argc, char **argv)
{
	// A CHANGER
	struct User current;
	bzero(current.id, BUFLEN);
	snprintf(current.id, BUFLEN, "JeanMichel");
	bzero(current.mdp, BUFLEN);
	snprintf(current.mdp, BUFLEN, "Crapeaud");

	char* strOpen = "Ouvert";
	char* strClose = "Ferme";
	char* strDone = "Fait";

	char* strComOpen = "Ouvrir";
	char* strComClose = "Fermer";
	char* strComState = "Etat";

	char commande[BUFLEN];
	char send;

	struct sockaddr_in si_other;
	size_t taille;
	unsigned int s;
	char buf[BUFLEN];

	BYTE iniKey[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
	WORD key[60]; // ????
	aes_key_setup(iniKey,key,256);
	BYTE iv[16] = {0x00, 0x11, 0x22, 0x13, 0x28, 0xae, 0xd2, 0xa6, 0xa0, 0xf7, 0x15, 0x88, 0x09, 0x78, 0x4f, 0x3c};

	unsigned char bufferOut[BUFLEN];
	unsigned char bufferIn[BUFLEN];

	if(argc != 2){
		printf("commande : ./client <ip>\n");
		return 0;
	}

	// ----- Authentification -----

	// Calcul de h(id)
	sha256((unsigned char*)current.id,strlen(current.id),bufferOut+1);
	bufferOut[0] = CLI_AUTH;
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
	// Envoi de hId en clair
	write(s, bufferOut, SHA256_BLOCK_SIZE+1);

	// Reception
	taille = read(s, bufferIn, BUFLEN);

	close(s);

	while(strcmp(commande,"quit") != 0){
		bzero(commande, BUFLEN);
		bzero(buf, BUFLEN);
		bzero(bufferOut, BUFLEN);
		bzero(bufferIn, BUFLEN);

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
			aes_encrypt_ctr((unsigned char*)buf,taille,bufferOut,key,256,iv);			
			// Envois
			write(s, bufferOut, taille);
			bzero(buf, BUFLEN);
			
			// Reception
			taille = read(s, bufferIn, BUFLEN);
			// Déchiffrement
			aes_decrypt_ctr(bufferIn,taille,(unsigned char*)buf,key,256,iv);

			printf("Reponse : %s\n", buf);

			// Deconnexion
			close(s);
		}
	}	
	return 0;
}
