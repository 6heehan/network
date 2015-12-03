//server.cpp
//# include "header.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/socket.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <string.h>

# define LISTEN_QUEUE_SIZE 10
# define THREAD_NUMBER 4
# define BUFFER_SIZE 1024
# define PORT 7000

//뮤텍스를 초기화 하시오.
pthread_mutex_t acceptMutex = PTHREAD_MUTEX_INITIALIZER;

void mutexDestroy(pthread_mutex_t *mutex) {
//뮤텍스를 제거하시오.
	pthread_mutex_destroy(mutex);
}

void *workerTask(void *argument) {
	int listenFD = *((int *) argument);
	struct sockaddr_in connectSocket;
	socklen_t connectSocketLength = sizeof(connectSocket);
	int connectFD;
	int integer1, integer2, sum;
	int readBytes, writtenBytes;
	char sendBuffer[BUFFER_SIZE];
	char receiveBuffer[BUFFER_SIZE];

	pthread_mutex_lock(&acceptMutex);
	connectFD = accept(listenFD, (struct sockaddr *)&connectSocket, &connectSocketLength);
	pthread_mutex_unlock(&acceptMutex);


	while (true) {
//뮤텍스를 이용해 accept를 보호하시오
//클라이언트와 통신하시오.

	readBytes = read(connectFD, receiveBuffer, BUFFER_SIZE);
	integer1 = atoi(receiveBuffer);
	for(int i=0; i<readBytes; i++)
	{
		receiveBuffer[i] = '\0';
	}

	readBytes = read(connectFD, receiveBuffer, BUFFER_SIZE);
	integer2 = atoi(receiveBuffer);

	sum = integer1 + integer2;
	sprintf(sendBuffer, "%d", sum);
	write(connectFD, sendBuffer, BUFFER_SIZE);

	//close(connectFD);
	}

	close(connectFD);

	return NULL;
}
int main() {
	struct sockaddr_in listenSocket;
	memset(&listenSocket, 0, sizeof(listenSocket));
	listenSocket.sin_family = AF_INET;
	listenSocket.sin_addr.s_addr = htonl(INADDR_ANY );
	listenSocket.sin_port = htons(PORT);
	
	int *listenFD = (int *)malloc(sizeof(int));
	*listenFD = socket(AF_INET, SOCK_STREAM, 0);
	
	if (bind(*listenFD, (struct sockaddr *) &listenSocket, sizeof(listenSocket)) == -1) {
		printf("Can not bind.\n");
		fflush(stdout);
		close(*listenFD);
		mutexDestroy(&acceptMutex);
		return -1;
	}

	if (listen(*listenFD, LISTEN_QUEUE_SIZE) == -1) {
		printf("Listen fail.\n");
		fflush(stdout);
		close(*listenFD);
		mutexDestroy(&acceptMutex);
		return -1;
	}

	printf("Waiting for clients...\n");

	fflush(stdout);
	pthread_t workers[THREAD_NUMBER];
	int threadNumber;

	for (threadNumber = 0; threadNumber < THREAD_NUMBER; threadNumber++) {
		int threadCreateResult;
		threadCreateResult = pthread_create(&workers[threadNumber], NULL, workerTask, listenFD);
		if (threadCreateResult != 0) {
			printf("Thread create error\n");
			fflush(stdout);
			break;
		}
	}

	for (int i = 0; i < threadNumber; i++) {
		pthread_join(workers[i], NULL);
	}

	close(*listenFD);
	mutexDestroy(&acceptMutex);
	return 0;
}
