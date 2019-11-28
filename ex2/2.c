#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>

#define MAX_SIZE (8192)

int main(int argc, char **argv){
	int rc;
	unsigned int prio = 10;
	mqd_t srv_m;
	mqd_t cli_m;
	char buf[MAX_SIZE];

	if(argc < 2){
		printf("Usage:\nServer: %s -s\nClient: %s -c /[queue_name]\n", argv[0], argv[0]);
		exit(1);
	}

	// server
	if(strcmp(argv[1], "-s") == 0){
		printf("Server started\n");
		int counter = 0;
		char msg[10];

		srv_m = mq_open("/server_queue", O_CREAT | O_RDWR, 0666, NULL);
		if(srv_m == (mqd_t)-1){
			perror("Couldn't create server queue");
		}

		while(1){
			rc = mq_receive(srv_m, buf, MAX_SIZE, &prio);
			if(rc == -1){
				perror("mq_receive");
			}
			printf("Server received: %s\n", buf);

			cli_m = mq_open(buf, O_RDWR, 0666, NULL);
			if(cli_m == (mqd_t)-1){
				perror("Couldn't open client queue");
			}

			sprintf(msg, "%d", counter++);

			rc = mq_send(cli_m, msg, strlen(msg), prio);
			if(rc == -1){
				perror("mq_send");
			}

		}


	}
	// client
	else if(strcmp(argv[1], "-c") == 0){
		printf("Client started\n");

		if(argc != 3){
			printf("Usage: %s -c /[queue_name]\n", argv[0]);
			exit(1);
		}

		cli_m = mq_open(argv[2], O_CREAT | O_RDWR, 0666, NULL);
		if(cli_m == (mqd_t)-1){
			perror("Couldn't create client queue");
		}

		srv_m = mq_open("/server_queue", O_RDWR, 0666, NULL);
		if(srv_m == (mqd_t)-1){
			perror("Couldn't open server queue");
		}

		rc = mq_send(srv_m, argv[2], strlen(argv[2]), prio);
		if(rc == -1){
			perror("mq_send");
		}

		rc = mq_receive(cli_m, buf, MAX_SIZE, &prio);
		if(rc == -1){
			perror("mq_receive");
		}
		printf("Client received: %s\n", buf);



	}

	return 0;
}