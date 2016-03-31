#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aes.h"
#include "sha256.h"
#include "network.h"

#define BUFLEN 512

struct User {
	char id[BUFLEN];
	char mdp[BUFLEN];
};

void setUser(struct User* u, char* id, char* mdp){
	snprintf(u->id, BUFLEN, id);
	snprintf(u->mdp, BUFLEN, mdp);
}

int main(int argc, char **argv)
{
	// A CHANGER
	struct User current;
	setUser(&current,"JeanMichel","Crapeaud");

	char commande[BUFLEN];
	TAG tag;
	char send;

	size_t taille;
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
	sha256((unsigned char*)current.id,strlen(current.id),bufferOut);

	//connexion au serveur
	if(netConnect(argv[1]) != 0){
		printf("Impossible de se connecter");
		return 0;
	}

	// Envoi de hId en clair
	tag = CLI_AUTH;
	netWrite(&tag, bufferOut, SHA256_BLOCK_SIZE);

	// Reception
	taille = netRead(&tag, bufferIn, BUFLEN);

	netDisconnect();

	while(strcmp(commande,"quit") != 0){
		bzero(commande, BUFLEN);
		bzero(buf, BUFLEN);
		bzero(bufferOut, BUFLEN);
		bzero(bufferIn, BUFLEN);

		printf("Commande : ");
		scanf("%512s", commande);
		send = 1;

		if(strcmp(commande,strComOpen) == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComOpen);
		else if(strcmp(commande,strComClose) == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComClose);
		else if(strcmp(commande,strComState) == 0)
			taille = snprintf(buf, BUFLEN, "%s",strComState);
		else if(strcmp(commande,"quit") == 0){
			send = 0;
		}
		else{
			printf("commandes : %s| %s | %s | quit\n",strComOpen,strComClose,strComState);
			send = 0;
		}

		if(send){
			//connexion au serveur
			if(netConnect(argv[1]) != 0){
				printf("Impossible de se connecter");
				return 0;
			}

			// Chiffrement
			aes_encrypt_ctr((unsigned char*)buf,taille,bufferOut,key,256,iv);			
			
			// Envois
			tag = CLI_MES;
			netWrite(&tag, bufferOut, taille);
			
			// Reception
			taille = netRead(&tag, bufferIn, BUFLEN);
			if(tag == SER_MES){
				// Déchiffrement
				bzero(buf, BUFLEN);
				aes_decrypt_ctr(bufferIn,taille,(unsigned char*)buf,key,256,iv);
				printf("Reponse : %s\n", buf);
			}
			else
				printf("Erreur de tag (%d reçu)\n", tag);

			// Deconnexion
			printf("---------------------------------\n");
			netDisconnect();
		}
	}	
	return 0;
}
