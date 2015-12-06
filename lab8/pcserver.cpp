//# include "header.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <errno.h>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <string.h>

# define LISTEN_QUEUE_SIZE 10
# define THREAD_NUMBER 4
# define BUFFER_SIZE 1024
# define PORT 7000

struct queue{
	int queue[THREAD_NUMBER];
	int writeIndex;
	int readIndex;
	int count;
} sharedQueue;
//뮤텍스와 컨디션을 초기화 하시오.

pthread_mutex_t conditionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t consumerCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t producerCondition = PTHREAD_COND_INITIALIZER;

void initializeQueue() {
	sharedQueue.readIndex = 0;
	sharedQueue.writeIndex = 0;
	sharedQueue.count = 0;
}

void mutexDestroy(pthread_mutex_t *mutex, pthread_cond_t *cond, pthread_cond_t *cond1) {
//뮤텍스와 컨디션을 제거하시오.
	pthread_mutex_destroy(mutex);
	pthread_cond_destroy(cond);
	pthread_cond_destroy(cond1);
}

void *workerTask(void *argument) {
	int connectFD;
	int integer1, integer2, sum;
	int readBytes, writtenBytes;
	char sendBuffer[BUFFER_SIZE];
	char receiveBuffer[BUFFER_SIZE];

	int n;
	struct sockaddr_in connectSocket;
	socklen_t connectSocketLength = sizeof(connectSocket);

	pthread_mutex_lock(&conditionMutex);
	if(sharedQueue.count == 0)
	{
		pthread_cond_wait(&producerCondition, &conditionMutex);
	}

	connectFD = sharedQueue.queue[sharedQueue.readIndex++];
	if(sharedQueue.readIndex == THREAD_NUMBER)
	{
		sharedQueue.readIndex = 0;
	}
	sharedQueue.count--;

	pthread_cond_signal(&consumerCondition);
	pthread_mutex_unlock(&conditionMutex);

	while (true) {
//프로듀서가 제공한 connect FD를 이용해 클라이언트와 통신하시오.


	n = read(connectFD, receiveBuffer, BUFFER_SIZE);
        receiveBuffer[n] = '\0';
        integer1 = atoi(receiveBuffer);

        n = read(connectFD, receiveBuffer, BUFFER_SIZE);
        receiveBuffer[n] = '\0';
        integer2 = atoi(receiveBuffer);

        sum = integer1 + integer2;
        sprintf(sendBuffer, "%d", sum);
        write(connectFD, sendBuffer, BUFFER_SIZE);	
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
	int listenFD = socket(AF_INET, SOCK_STREAM, 0);
	
	if (bind(listenFD, (struct sockaddr *) &listenSocket, sizeof(listenSocket)) == -1) {
		printf("Can not bind.\n");
		fflush(stdout);
		close(listenFD);
		mutexDestroy(&conditionMutex, &consumerCondition, &producerCondition);
		return -1;
	}
	
	if (listen(listenFD, LISTEN_QUEUE_SIZE) == -1) {
		printf("Listen fail.\n");
		fflush(stdout);
		close(listenFD);
		mutexDestroy(&conditionMutex, &consumerCondition, &producerCondition);
		return -1;
	}

	printf("Waiting for clients...\n");
	fflush(stdout);

	struct sockaddr_in connectSocket;
	socklen_t connectSocketLength = sizeof(connectSocket);
	int connectFD;
	initializeQueue();
	pthread_t workers[THREAD_NUMBER] = {0};
	int threadNumber;

	for (threadNumber = 0; threadNumber < THREAD_NUMBER; threadNumber++) {
		int threadCreateResult;
		threadCreateResult = pthread_create(&workers[threadNumber], NULL, workerTask, NULL);
		if (threadCreateResult != 0) {
			printf("Thread create error\n");
			fflush(stdout);
			break;
		}
	}

	while (true) {
	connectFD = accept(listenFD, (struct sockaddr*)&connectSocket, &connectSocketLength);
//connect FD를 공유 큐에 추가하여 컨슈머가 사용할 수 있도록 하시오.
	
	pthread_mutex_lock(&conditionMutex);
	if(sharedQueue.count >= THREAD_NUMBER)
	{
		pthread_cond_wait(&consumerCondition, &conditionMutex);
	}

	sharedQueue.queue[sharedQueue.writeIndex++] = connectFD;
	if(sharedQueue.writeIndex == THREAD_NUMBER)
	{
		sharedQueue.writeIndex = 0;
	}

	sharedQueue.count++;
	pthread_cond_signal(&producerCondition);
	pthread_mutex_unlock(&conditionMutex);
	
	}

	for (int i = 0; i < threadNumber; i++) {
		pthread_join(workers[i], NULL);
	}

	close(listenFD);
	mutexDestroy(&conditionMutex, &consumerCondition, &producerCondition);
	return 0;
}
