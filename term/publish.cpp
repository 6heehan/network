# include "topic1.h"

int main(int argc, char ** argv)
{
    struct sockaddr_in connectsocket;
    char buffer[BUFFER_SIZE];
    int i, j, k, l=0;
    struct topic a;
    ssize_t n;
    char** name;

    memset(&connectsocket, 0, sizeof(connectsocket));
    connectsocket.sin_family = AF_INET;
    inet_aton("127.0.0.1", (struct in_addr *)&connectsocket.sin_addr.s_addr);
    connectsocket.sin_port = htons(PORT);

    int connectFD = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(connectFD, (struct sockaddr *)&connectsocket, sizeof(connectsocket)) == -1)
    {
        printf("Can not connect.\n");
        return -1;
    }
    else
    {
        write(connectFD, argv[0], BUFFER_SIZE);
        n = read(connectFD, buffer, BUFFER_SIZE);
        buffer[n] = '\0';
        //등록되어있는 publish수를 받음
        k = atoi(buffer);
        name = (char **)malloc(sizeof(char *)*k);

        for(i=0; i<k; i++)
        {	//동적할당 후 토픽 이름 저장
            name[i] = (char *)malloc(sizeof(char)*BUFFER_SIZE);
            n = read(connectFD, name[i], BUFFER_SIZE);
            name[i][n] = '\0';
        }

        for(i=0; i<k; i++)
        {
            if(strcmp(name[i], argv[1])==0)
            {
                printf("토픽이 중복되었습니다.\n");
                write(connectFD, NULL, BUFFER_SIZE);
                close(connectFD);
                return -1;
            }
        }

        write(connectFD, argv[1], BUFFER_SIZE);
        j = 0;
        printf("connection sucess topic : %s\n",argv[1]);
        printf("publish topic : %s\n",argv[1]);
        while(1)
        {
            makemessage(&a, j);
            write(connectFD, a.message, BUFFER_SIZE);
            j++;
            sleep(3);
        }
    }
    close(connectFD);
    return 0;
}
