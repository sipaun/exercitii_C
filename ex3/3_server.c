#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "utils.h"

void *thread_main(void *arg);

struct threadArgs{
	int cli_sock;
};

void handle_client(int clnt_sock);

int main(int argc, char **argv){
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	unsigned short server_port;
	unsigned int clnt_len;
	pthread_t thread_id;
	struct threadArgs *thread_args;

	if(argc != 2){
		printf("Usage: %s [port]\n", argv[0]);
		exit(1);
	}

	server_port = strtol(argv[1], NULL, 10);

	if((serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		perror("socket() failed\n");
		exit(1);
	}

	if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
		perror("setsockopt() failed\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(server_port);

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind() failed\n");
		exit(1);
	}

	if(listen(serv_sock, 5) < 0){
		perror("listen() failed\n");
		exit(1);
	}

	while(1){
		clnt_len = sizeof(clnt_addr);
		if((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0){
			perror("accept() failed\n");
			exit(1);
		}

		printf("Handling client %s:%d\n", inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);

		if((thread_args = (struct threadArgs*)malloc(sizeof(struct threadArgs))) == NULL){
			perror("malloc() failed\n");
			exit(1);
		}
		thread_args->cli_sock = clnt_sock;

		if(pthread_create(&thread_id, NULL, thread_main, (void*)thread_args) != 0){
			perror("pthread_create() failed\n");
			exit(1);
		}

		
	}
	close(serv_sock);
}

void handle_client(int clnt_sock){
	char buffer[100];
	int recv_size = 0;
	int size;
	int file_found;
	int file_size;

	//recv size
	if((recv_size = recv(clnt_sock, &size, sizeof(int), 0)) < 0){
		perror("recv() size failed\n");
		exit(1);
	}

	size = ntohl(size);

	printf("Size: %d\n", size);

	// recv message
	if(recvall(clnt_sock, buffer, size) != 0){
		perror("recvall() message failed\n");
		exit(1);
	}

	printf("File to send: %s\n", buffer);

	if(access(buffer, F_OK) != -1){
		file_found = 1;
	}
	else{
		file_found = 0;
	}

	if(buffer[0] == '.' || buffer[0] == '/'){
		file_found = 0;
	}


	if(file_found){
		char *resp;
		FILE *f;

		if((f = fopen(buffer, "rb")) == NULL){
			perror("fopen() failed");
			exit(1);
		}

		fseek(f, 0L, SEEK_END);
		file_size = ftell(f);
		rewind(f);

		resp = (char*)malloc(file_size);
		fread(resp, 1, file_size, f);
		fclose(f);

		file_size = htonl(file_size);
		// send size
		if(sendall(clnt_sock, (char*) &file_size, sizeof(int)) != 0){
			perror("sendall() size failed\n");
			exit(1);
		}

		file_size = ntohl(file_size);

		// send message back
		if(sendall(clnt_sock, resp, file_size) != 0){
			perror("sendall() message failed\n");
			exit(1);
		}
	}
	else{
		file_size = htonl(0);
		// send size
		if(sendall(clnt_sock, (char*) &file_size, sizeof(int)) != 0){
			perror("sendall() size failed\n");
			exit(1);
		}
	}

	printf("Done\n");
	close(clnt_sock);
}

void *thread_main(void *arg){
	int clnt_sock;

	pthread_detach(pthread_self());

	clnt_sock = ((struct threadArgs*)arg)->cli_sock;
	free(arg);

	handle_client(clnt_sock);

	return (NULL);
}
