#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "network.h"

#define BUFLEN 512

void buffRand(char* buffer, size_t bufferSize){
	int i;
	for(i=0; i<bufferSize; i++){
		buffer[i] = rand();
	}
}

int main(int argc, char **argv)
{
	size_t taille;
	char buf[BUFLEN];
	TAG tag;
	int i,j;

	if(argc != 2){
		printf("commande : ./client <ip>\n");
		return 0;
	}

	printf("---------------------------------\n");

	// ----- Fin Authentification -----
	j = 0;
	while(1){
		//connexion au serveur
		if(netConnect(argv[1]) == 0){
			j++;
			taille = rand()*512/RAND_MAX;
			buffRand(buf, taille);
			// Envois
			tag = CLI_AUTH;
			netWrite(&tag,buf,taille);
			//netWriteRaw(buf,BUFLEN);
			
			// // Reception
			// taille = netRead(&tag, bufferIn, BUFLEN);
			// if(tag == SER_MES){
			// 	// Déchiffrement
			// 	bzero(buf, BUFLEN);
			// 	increment_iv(iv,16);
			// 	aes_decrypt_ctr(bufferIn,taille,(unsigned char*)buf,key,256,iv);
			// 	printf("Reponse : %s\n", buf);
			// }
			// else
			// 	printf("Erreur de tag (%d reçu)\n", tag);

			// Deconnexion
			printf("Envois %d\n",j);
			netDisconnect();
		}
	}	
	return 0;
}
