#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "network.h"
#include "aes.h"
#include "sha256.h"

#define BUFLEN 512

typedef unsigned char TAG;

// struct User {
// 	BYTE hId[SHA256_BLOCK_SIZE] = {0x7a,0xb2,0x8b,0xe1,0x45,0x36,0x7c,0x0c,0xc6,0x1d,0x3b,0xb0,0x3f,0x34,0x7c,0x07,0xe5,0x65,0x2e,0xac,0x9d,0xe4,0xd6,0xec,0xd3,0x41,0x18,0x59,0x4d,0x79,0x2d,0x9b};
// 	BYTE hMpdSel[SHA256_BLOCK_SIZE] = {0xa0,0x6d,0x66,0xc5,0x65,0x17,0x1c,0x64,0x7d,0xfc,0x13,0x6b,0x3d,0x94,0x83,0xef,0xaf,0x21,0x90,0x0a,0x38,0xa1,0x5d,0xe4,0xa6,0x59,0x15,0x6a,0xd6,0xb6,0x01,0xf2};
// 	BYTE sel[SHA256_BLOCK_SIZE] = {0x30,0x5b,0x8c,0xe1,0xb7,0x91,0xe0,0xdf,0xa0,0x6b,0xc1,0x3b,0xc1,0x6c,0xfa,0xf0,0x08,0x79,0xb6,0xa0,0x3a,0x50,0x79,0xdd,0xdf,0x50,0x51,0xbd,0xe4,0x5a,0x13,0x90};
// };

int setState(int state){
	time_t tA;
	time_t tB;

	tA = time(0);
	if(state == STATE_OPEN){
		//system("eject");
		printf("system(eject)\n");
	}
	else if(state == STATE_CLOSE){
		//system("eject -t");
		printf("system(eject -t)\n");
	}
	tB = time(0);

	return tB - tA;
}

int main(void)
{
	BYTE hId[SHA256_BLOCK_SIZE] = {0x7a,0xb2,0x8b,0xe1,0x45,0x36,0x7c,0x0c,0xc6,0x1d,0x3b,0xb0,0x3f,0x34,0x7c,0x07,0xe5,0x65,0x2e,0xac,0x9d,0xe4,0xd6,0xec,0xd3,0x41,0x18,0x59,0x4d,0x79,0x2d,0x9b};
	BYTE hMpdSel[SHA256_BLOCK_SIZE] = {0xa0,0x6d,0x66,0xc5,0x65,0x17,0x1c,0x64,0x7d,0xfc,0x13,0x6b,0x3d,0x94,0x83,0xef,0xaf,0x21,0x90,0x0a,0x38,0xa1,0x5d,0xe4,0xa6,0x59,0x15,0x6a,0xd6,0xb6,0x01,0xf2};
	BYTE sel[SHA256_BLOCK_SIZE] = {0x30,0x5b,0x8c,0xe1,0xb7,0x91,0xe0,0xdf,0xa0,0x6b,0xc1,0x3b,0xc1,0x6c,0xfa,0xf0,0x08,0x79,0xb6,0xa0,0x3a,0x50,0x79,0xdd,0xdf,0x50,0x51,0xbd,0xe4,0x5a,0x13,0x90};
	
	char* temp;

	size_t taille;

	char buf[BUFLEN];
	char open;
	TAG tag;

	time_t t;

	BYTE iniKey[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
	WORD key[60]; // ????
	aes_key_setup(iniKey,key,256);
	BYTE iv[16] = {0x00, 0x11, 0x22, 0x13, 0x28, 0xae, 0xd2, 0xa6, 0xa0, 0xf7, 0x15, 0x88, 0x09, 0x78, 0x4f, 0x3c};
	
	unsigned char bufferOut[BUFLEN+1];
	unsigned char bufferIn[BUFLEN+1];

	//AES128_CBC_encrypt_buffer(buffer, in, 16, key ,iv);

	// Initialisation du serveur
	if(netServInit() != 0){
		printf("Impossible d'initialiser le serveur");
		return 0;
	}

	//initialisation du lecteur
	setState(STATE_CLOSE);
	open = 0;

	while(1){
		netServListen();
		if(netServAccept()) {
			bzero(buf, BUFLEN);
			bzero(bufferIn, BUFLEN);
			bzero(bufferOut, BUFLEN);

			// Réception
			taille = netRead(&tag,bufferIn,BUFLEN);

			if(tag == CLI_AUTH){
				printf("Authentification\n");
				if(!memcmp(hId,bufferIn,32)){
					printf("Identifiant valide\n");
					// A FAIRE
				} else{
					printf("Identifiant invalide\n");
				}
			}
			else if(tag == CLI_MES){
				aes_decrypt_ctr(bufferIn,taille,(unsigned char*)buf,key,256,iv);

				// Traitement
				printf("Commande : %s\n", buf);

				// OUVERTURE
				if(strcmp(buf,strComOpen) == 0){
					if(setState(STATE_OPEN) > 0 && open == STATE_OPEN)
						taille = snprintf(buf, BUFLEN, "%s (systeme force)", strDone);
					else if(open == STATE_OPEN)
						taille = snprintf(buf, BUFLEN, "%s (deja %s)", strDone, strOpen);
					else
						taille = snprintf(buf, BUFLEN, "%s", strDone);
					open = STATE_OPEN;
				}
				// FERMETURE
				else if(strcmp(buf,strComClose) == 0){
					if(setState(STATE_CLOSE) > 0 && open == STATE_CLOSE)
						taille = snprintf(buf, BUFLEN, "%s (systeme force)", strDone);
					else if(open == STATE_CLOSE)
						taille = snprintf(buf, BUFLEN, "%s (deja %s)", strDone, strClose);
					else
						taille = snprintf(buf, BUFLEN, "%s", strDone);
					open = STATE_CLOSE;
				}
				// ETAT
				else if(strcmp(buf,strComState) == 0){
					if(open){
						t = setState(STATE_OPEN);
						temp = strOpen;
					}
					else{
						t = setState(STATE_CLOSE);
						temp = strClose;
					}
					if(t > 0)
						taille = snprintf(buf, BUFLEN, "%s (systeme force)", temp);
					else
						taille = snprintf(buf, BUFLEN, "%s", temp);
				}
				else{
					taille = snprintf(buf, BUFLEN, "Error");
				}

				printf("Réponse : %s\n",buf);

				// Chiffrement
				aes_encrypt_ctr((unsigned char*)buf,taille,bufferOut,key,256,iv);
				// Réponse
				tag = SER_MES;
				netWrite(&tag,bufferOut,taille);
			}else{
				printf("Erreur de tag (%d reçu)\n", tag);
			}

			// Fin de connexion
			printf("- Fin de connexion\n\n");
			netDisconnect();
		}
	}
	netServDisconnect();
	return 0;
}
