#include <sys/socket.h>

int sendall(int s, char *buf, int len){
	int total = 0;
	int bytesleft = len;
	int n;

	while(total < len){
		n = send(s, buf+total, bytesleft, 0);
		if(n == -1) break;
		total += n;
		bytesleft -= n;
	}

	return n==-1?-1:0;
}

int recvall(int s, char *buf, int len){
	int total = 0;
	int bytesleft = len;
	int n;

	while(total < len){
		n = recv(s, buf+total, bytesleft, 0);
		if(n == -1) break;

		total += n;
		bytesleft -= n;
	}


	return n==-1?-1:0;
}