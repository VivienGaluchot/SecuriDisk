#include "network.h"

unsigned int sock = 0;
unsigned int servSock = 0;

int netServInit(){
	struct sockaddr_in si_me;
	unsigned int taille;

	servSock = socket(AF_INET, SOCK_STREAM, 0);
	if(servSock == -1)
		return -1;
	taille = 1;
	if( (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &taille, sizeof(int))) == -1 )
		return -1;
	memset((char*)&si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(servSock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1 )
		return -2;
	return 0;
}


void netServListen(){
	listen(servSock, 5);
}


int netServAccept(){	
	struct sockaddr_in si_other;
	size_t slen = sizeof(si_other);
	sock = accept(servSock, (struct sockaddr * __restrict__) &si_other, (socklen_t * __restrict__) &slen);
	if(sock > 0){
		printf("- Connexion de %s:%d\n", inet_ntoa(si_other.sin_addr),ntohs(si_other.sin_port));
		return 1;
	}
	printf("Erreur %d - %s\n",errno,strerror(errno));
	return 0;
}

void netServDisconnect(){
	if(servSock != 0)
		close(servSock);
	servSock = 0;
}

int netConnect(char* ip){	
	struct sockaddr_in si_other;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1 )
		return -1;
	bzero(&si_other, sizeof(struct sockaddr_in));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	if( inet_aton(ip, &si_other.sin_addr)==0 )
		return -2;
	if( connect(sock, (struct sockaddr*)&si_other, sizeof(struct sockaddr)) < 0 )
		return -3;
	return 0;
}

void netWrite(TAG* t, unsigned char* buffer, size_t bufferSize){
	write(sock, t, 1);
	write(sock, buffer, bufferSize);
}

size_t netRead(TAG* t, unsigned char* buffer, size_t bufferSize){
	read(sock, t, 1);
	return read(sock, buffer, bufferSize);
}

void netDisconnect(){
	if(sock != 0)
		close(sock);
	sock = 0;
}