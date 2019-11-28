CC=gcc
CFLAGS=-Wall -g
OBJ_S = 3_server.c utils.c
OBJ_C = 3_client.c utils.c

all: 3_server 3_client

3_server:$(OBJ_S)
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

3_client:$(OBJ_C)
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

clean:
	rm 3_client 3_server