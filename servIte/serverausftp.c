#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "serverdtp.h"

#define VERSION "1.0"
#define PTODEFAULT 21
#define PWDFILE "/etc/ausftp/ftpusers"


int main(int argc, char const *argv[]){

	int port;

	if(argc > 2){
		fprintf(stderr, "Error: mal ingreso de argumentos\n");
		return 1;
	}

	if(argc == 2){
		port = atoi(argv[1]);
	}else{
		port = 21; //puerto default ftp
	}

	if(port == 0){
		fprintf(stderr, "Error, puerto invalido \n");
		return 1;
	}

	printf("%d\n", port);
	
	printf("devuelve %d\n", check_credentials("pepe", "pepe"));
	
	int mastersocket, slavesocket;
	struct sockaddr_in masteraddr, slaveaddr;
	socklen_t slaveaddrlen;

	mastersocket = socket(AF_INET, SOCK_STREAM, 0);

	masteraddr.sin_family = AF_INET;
	masteraddr.sin_addr.s_addr = INADDR_ANY;
	masteraddr.sin_port = htons(port);

	bind(mastersocket, (struct sockaddr*)&masteraddr,sizeof(masteraddr));

	listen(mastersocket, 5);

	while(true){
		slaveaddrlen = sizeof(slaveaddr);
		slavesocket = accept(mastersocket, (struct sockaddr*) &slaveaddr, &slaveaddrlen);
		send(slavesocket, "220 1", sizeof("220 1"), 0);
		printf("funciono\n");
	}
	
	close(mastersocket);

	return 0;
}
