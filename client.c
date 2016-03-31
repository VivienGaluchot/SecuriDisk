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

void ouExclusiveur(char* mdp, BYTE* sel, size_t selSize, BYTE* buffer){
	int i;
	char strFini = 0;
	for(i=0; i<selSize; i++){
		if(mdp[i] == 0) strFini = 1;
		if(!strFini){
			buffer[i] = sel[i] ^ mdp[i];
		}
		else{
			buffer[i] = sel[i];
		}
	}
}

int main(int argc, char **argv)
{
	struct User current;

	char commande[BUFLEN];
	TAG tag;
	char send;

	size_t taille;
	char buf[BUFLEN];

	BYTE iniKey[32];
	BYTE iv[16];
	WORD authKey[60]; // ????
	WORD key[60]; // ????
	BYTE sel[32];

	unsigned char bufferOut[BUFLEN];
	unsigned char bufferIn[BUFLEN];

	if(argc != 2){
		printf("commande : ./client <ip>\n");
		return 0;
	}

	// ----- Authentification -----

	printf("Identifiant : ");
	scanf("%s",current.id);

	printf("Mot de passe : ");
	scanf("%s",current.mdp);

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
	// Sel
	taille = netRead(&tag, sel, 32);
	if(taille != 32 || tag != SER_AUTH) return 0;

	// IV
	taille = netRead(&tag, iv, 16);
	if(taille != 16 || tag != SER_AUTH) return 0;

	// Clé de session chiffrée
	taille = netRead(&tag, bufferIn, 32);
	if(taille != 32 || tag != SER_AUTH) return 0;

	// Calcul de la clé d'authentification
	ouExclusiveur(current.mdp, sel, 32, (unsigned char*) buf);
	sha256((unsigned char*) buf,32,bufferOut);
	// CA CHIE ICI
	aes_key_setup(bufferOut,authKey,256);

	// Déchiffrement
	aes_decrypt_ctr(bufferIn,32,iniKey,authKey,256,iv);
	aes_key_setup(iniKey,key,256);

	netDisconnect();

	printf("---------------------------------\n");

	// ----- Fin Authentification -----

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
				printf("Impossible de se connecter\n");
				return 0;
			}

			// Chiffrement
			increment_iv(iv,16);
			aes_encrypt_ctr((unsigned char*)buf,taille,bufferOut,key,256,iv);			
			
			// Envois
			tag = CLI_MES;
			netWrite(&tag, bufferOut, taille);
			
			// Reception
			taille = netRead(&tag, bufferIn, BUFLEN);
			if(tag == SER_MES){
				// Déchiffrement
				bzero(buf, BUFLEN);
				increment_iv(iv,16);
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
