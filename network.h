#ifndef NETWORK_H
#define NETWORK_H

#define PORT 25655

typedef unsigned char TAG;

// Valeur des tags
#define CLI_AUTH 1
#define SERV_AUTH 2
#define CLI_MES 3
#define	SER_MES 4

// Etat du lecteur
#define STATE_OPEN 1
#define STATE_CLOSE 0

// Resultats
static char* strOpen = "Ouvert";
static char* strClose = "Ferme";
static char* strDone = "Fait";
// Commandes
static char* strComOpen = "Ouvrir";
static char* strComClose = "Fermer";
static char* strComState = "Etat";

#endif   // NETWORK_H