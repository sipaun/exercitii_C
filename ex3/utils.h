
#pragma once

/**
	Repetedly calls send() until all data is sent.
	@param s: socket fd
	@param buf: pointer to the source of the data
	@param len: length of the data
	@return 0 if success, -1 if failed
*/

int sendall(int s, char *buf, int len);

/**
	Repetedly calls recv() until all data is received.
	@param s: socket fd
	@param buf: pointer to the destination memory
	@param len: length of data to receive
	@return 0 if success, -1 if failed
*/

int recvall(int s, char *buf, int len);