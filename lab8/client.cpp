//client.cpp
//# include "header.h"
# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>

# define BUFFER_SIZE 1024
# define PORT 7000

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage: %s IPv4-address\n", argv[0]);
		return -1;

	}

	struct sockaddr_in connectSocket;
	memset(&connectSocket, 0, sizeof(connectSocket));
	connectSocket.sin_family = AF_INET;
	inet_aton(argv[1], (struct in_addr*) &connectSocket.sin_addr.s_addr);
	connectSocket.sin_port = htons(PORT);
	int connectFD = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(connectFD, (struct sockaddr*) &connectSocket, sizeof(connectSocket)) == -1) {
		printf("Can not connect.\n");
		return -1;
	}

	int integer1, integer2, inputValidation;
	int readBytes, writtenBytes;
	char sendBuffer[BUFFER_SIZE];
	char receiveBuffer[BUFFER_SIZE];

	while (true) {
		printf("Input: ");
		fflush(stdout);
		inputValidation = scanf("%d %d", &integer1, &integer2);
		if (inputValidation < 2) {
			printf("Inpur error\n");
			fflush(stdout);
			break;
		}
//입력 받은 두 정수를 서버에 전송하시오.
//서버가 전송한 두 정수의 합을 받으시오.

	sprintf(sendBuffer, "%d", integer1);
	write(connectFD, sendBuffer, BUFFER_SIZE);
	sprintf(sendBuffer, "%d", integer2);
	write(connectFD, sendBuffer, BUFFER_SIZE);

	read(connectFD, receiveBuffer, BUFFER_SIZE);

	printf("Result: %d + %d = %s\n", integer1, integer2, receiveBuffer);
	fflush(stdout);
	}	

	close(connectFD);
	return 0;
}
