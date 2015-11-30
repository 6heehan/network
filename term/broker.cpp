#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define CLIENTMAX 3
#define PORT 7000
#define BUFFER_SIZE 1024


struct client
{
	int sock_fd;
	struct sockaddr_in sock_adr;
	char topicname[BUFFER_SIZE];
};

struct client* publish_client[CLIENTMAX];
int publish_idx = 0;
struct client* subscribe_client[CLIENTMAX];
int subscribe_idx = 0;

int maxfds = 0;
fd_set reset;

void add_client(struct client* clnt);
void remove_client(int sock_fd);
struct client* get_client(int sock_fd, int* idx);
void topic_write(char* data, int datalen, char* topicname);

int main()
{

	int serv_sock, ret;
	struct sockaddr_in listenSocket;

	FD_ZERO(&reset);

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&listenSocket, 0, sizeof(listenSocket));

	listenSocket.sin_family = AF_INET;
	listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	listenSocket.sin_port = htons(PORT);

	if(bind(serv_sock, (struct sockaddr*)&listenSocket, sizeof(listenSocket)) == -1)
	{
		printf("Can not bind.\n");
		return -1;
	}

	if(listen(serv_sock, SOMAXCONN) == -1)
	{
		printf("Listen fail.\n");
		return -1;
	}

	printf("Waiting for clients..\n");

		
