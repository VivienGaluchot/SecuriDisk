#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 25655

typedef unsigned char TAG;

// Valeur des tags
#define CLI_AUTH 1
#define SERV_AUTH 2
#define CLI_MES 3
#define	SER_MES 4

// Code d'information
#define STATE_CLOSE 0
#define STATE_OPEN 1
// #define DONE 2
// #define DO_OPEN 10
// #define DO_CLOSE 11
// #define DO_STATE 12

// Resultats
static char* strOpen = "Ouvert";
static char* strClose = "Ferme";
static char* strDone = "Fait";
// Commandes
static char* strComOpen = "Ouvrir";
static char* strComClose = "Fermer";
static char* strComState = "Etat";

int netServInit();
void netServListen();
int netServAccept();
void netServDisconnect();

int netConnect(char* ip);
void netWrite(TAG* t, unsigned char* buffer, size_t bufferSize);
size_t netRead(TAG* t, unsigned char* buffer, size_t bufferSize);
void netDisconnect();

#endif   // NETWORK_H