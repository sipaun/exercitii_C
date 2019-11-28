#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char **argv){
	int clnt_sock;
	struct sockaddr_in serv_addr;
	unsigned short server_port;

	if(argc != 4){
		printf("Usage: %s [srv_addr] [srv_port] [file]\n", argv[0]);
		exit(0);
	}

	server_port = strtol(argv[2], NULL, 10);
	int message_size = strlen(argv[3]) + 1;

	if((clnt_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		perror("socket() failed\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(server_port);

	if(connect(clnt_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("connect() failed\n");
		exit(1);
	}

	//send size
	int size = htonl(message_size);
	if(sendall(clnt_sock, (char*) &size, sizeof(int)) != 0){
		perror("sendall() size failed\n");
		exit(1);
	}


	//send message
	if(sendall(clnt_sock, argv[3], message_size) != 0){
		perror("sendall() message failed\n");
		exit(1);
	}

	int recv_size;

	// recv size
	if((recv_size = recv(clnt_sock, &size, sizeof(int), 0)) < 0){
		perror("recv() size failed\n");
		exit(1);
	}


	size = ntohl(size);

	printf("Size: %d\n", size);

	if(size == 0){
		printf("File NOT found!\n");
	}
	else{
		// recv message back
		char *resp;
		resp = (char*)malloc(size);

		FILE *f;

		if((f = fopen(argv[3], "wb+")) == NULL){
			perror("fopen() failed");
			exit(1);
		}

		if(recvall(clnt_sock, resp, size) != 0){
			perror("recvall() message failed\n");
			exit(1);
		}

		fwrite(resp, 1, size, f);
		fclose(f);
	}

	

	close(clnt_sock);
}