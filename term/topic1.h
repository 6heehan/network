#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define PORT 7000
#define MAXCLIENT 6

struct client
{
	int sock_fd;
	struct sockaddr_in sock_addr;
	char topicname[BUFFER_SIZE];
	char clientname[BUFFER_SIZE];
    bool flag;
};

struct topic
{
        char message[BUFFER_SIZE];
        char name[BUFFER_SIZE];
        bool flag;
};

char* timestamp()
{
        time_t ltime;
        ltime = time(NULL);
//      printf("%s",asctime(localtime(&ltime)));
        return asctime(localtime(&ltime));
}

void makemessage(struct topic* a, int i)
{
        char temp[BUFFER_SIZE];

        sprintf(temp, "%d", i);

        for(int j = 0; j < strlen(a->message); j++)
        {
                a->message[j] = '\0';
        }

        strcat(a->message, temp);
        strcat(a->message, " 번째 메세지 입니다 ");
        strcat(a->message, timestamp());

}

void maketopic(struct topic* a)
{
        printf("topic name : ");
        scanf("%s",a->name);
}

