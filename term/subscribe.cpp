# include "topic1.h"
bool msg;
void handler(int signal)
{
    if(!msg)
    {
        alarm(5);
        printf("reset topic\n");
    }

}
int main(int argc, char ** argv)
{
    struct sockaddr_in connectsocket;
    char buffer[BUFFER_SIZE];
    int i,j,k,l = 0;
    struct topic a;
    ssize_t n;

    signal(SIGALRM, handler);
    memset(&connectsocket, 0 ,sizeof(connectsocket));
    connectsocket.sin_family = AF_INET;
    inet_aton("127.0.0.1", (struct in_addr *)&connectsocket.sin_addr.s_addr);
    connectsocket.sin_port = htons(PORT);

    int connectFD = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(connectFD, (struct sockaddr *)&connectsocket, sizeof(connectsocket)) == -1)
    {
        printf("Can not bind\n");
        return -1;
    }
    else
    {
        printf("subscribe topic : %s\n",argv[1]);
        msg = false;
        write(connectFD, argv[0], BUFFER_SIZE);
        write(connectFD, argv[1], BUFFER_SIZE);

        while(1)
        {
            alarm(5);
            msg = false;
            n =  read(connectFD, buffer, BUFFER_SIZE);
            if(n == 0)
            {
                printf("connection lost\n");
                break;
            }
            buffer[n] = '\n';
            msg = true;
            printf("%s message : %s",argv[1], buffer);
        }
    }
    close(connectFD);
    return 0;
}
