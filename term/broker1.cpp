# include "topic1.h"

struct client* clnt_pub[3];
int clnt_pub_idx = 0;
struct client* clnt_sub[3];
int clnt_sub_idx = 0;
struct topic* topic_arr[3];
int topic_idx = 0;

int main()
{
    int i, j, k, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[MAXCLIENT];
    ssize_t n;
    fd_set rset, all_rset, wset, all_wset;
    char line[BUFFER_SIZE];
    char writebuffer[BUFFER_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("Can not bind.\n");
        return -1;
    }

    if(listen(listenfd, SOMAXCONN) == -1)
    {
        printf("Listen fail.\n");
        return -1;
    }

    printf("Waiting for clients..\n");

    maxfd = listenfd;
    maxi = -1;

    for(i=0; i<MAXCLIENT; i++)
        client[i] = -1;

    FD_ZERO(&all_rset);
    FD_ZERO(&all_wset);
    FD_SET(listenfd, &all_rset);


    while(1)
    {
        rset = all_rset;
        wset = all_wset;
        nready = select(maxfd+1, &rset, &wset, NULL, NULL);

        if(FD_ISSET(listenfd, &rset))
        {	//new connection 새로운 클라이언트가 접속될 때
            struct client* clnt;
            clnt = (struct client *)malloc(sizeof(struct client));
            struct topic* top;
            top = (struct topic *)malloc(sizeof(struct topic));

            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);

            for(i=0; i<MAXCLIENT; i++)
            {
                if(client[i]<0)
                {
                    client[i] = connfd;
                    break;
                }

            }

            if(i==MAXCLIENT)
            {
                printf("too many clients\n");
                exit(1);
            }

            n = read(connfd, line, BUFFER_SIZE);
            line[n] = '\0';

            //클라이언트의 종류를 구분


            if(strcmp(line, "./publish") == 0)
            {
                if(clnt_pub_idx > 2)
                {
                    printf("publish MAX : 3\n");
                    client[i] = -1;
                    close(connfd);
                    continue;
                }


                sprintf(writebuffer, "%d", clnt_pub_idx);
                write(connfd, writebuffer, BUFFER_SIZE);
                //등록되어있는 토픽 개수를 먼저 전송

                for(j=0; j<clnt_pub_idx; j++)
                {	//등록되어있는 토픽 이름 정보를 보냄
                    write(connfd, clnt_pub[j]->topicname, BUFFER_SIZE);
                }

                n = read(connfd, line, BUFFER_SIZE);
                if(n==0)
                {
                    client[i] = -1;
                    continue;
                }
                line[n] = '\0';
                //topic 을 받아옴

                clnt->sock_fd = connfd;
                clnt->sock_addr = cliaddr;
                strcpy(clnt->topicname, line);
                strcpy(clnt->clientname, "publish");
                clnt->flag = false;
                clnt_pub[clnt_pub_idx++] = clnt;

                //현재 등록하는 publish의 정보를 저장

                strcpy(top->name, line);
                strcpy(top->message, "\0");
                top->flag = false;
                topic_arr[topic_idx++] = top;
                //topic 등록

                FD_SET(connfd, &all_rset);

            }

            else if(strcmp(line, "./subscribe") == 0)
            {
                if(clnt_sub_idx > 2)
                {
                    printf("subscribe MAX : 3\n");
                    client[i] = -1;
                    close(connfd);
                    continue;
                }
                n = read(connfd, line, BUFFER_SIZE);
                line[n] = '\0';

                clnt->sock_fd = connfd;
                clnt->sock_addr = cliaddr;
                strcpy(clnt->topicname, line);
                strcpy(clnt->clientname, "subscribe");
                clnt->flag = false;
                clnt_sub[clnt_sub_idx++] = clnt;
                FD_SET(connfd, &all_wset);

            }





            if(connfd > maxfd)
                maxfd = connfd;

            if(i > maxi)
                maxi = i;

            if(--nready <= 0)
            {
                continue;
            }


        }


        for(i=0; i <= maxi; i++)
        {

            if((sockfd = client[i]) < 0)
                continue;

            if(FD_ISSET(sockfd, &rset))
            {

                for(j=0; j<clnt_pub_idx; j++)
                {
                    if(sockfd == clnt_pub[j]->sock_fd)
                        break;
                }

                if((n=read(sockfd, line, BUFFER_SIZE)) == 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &all_rset);
                    FD_CLR(sockfd, &all_wset);
                    client[i] = -1;
                }
                else
                {
                    line[n] = '\0';
                    for(k=0; k<topic_idx; k++)
                    {
                        if(strcmp(clnt_pub[j]->topicname, topic_arr[k]->name) == 0)
                            break;
                    }

                    strcpy(topic_arr[k]->message, line);
                    topic_arr[k]->flag = true;
                    printf("topic : %s message : %s",topic_arr[k]->name, topic_arr[k]->message);

                    for(k=0; k<clnt_sub_idx; k++)
                    {
                        if(strcmp(clnt_pub[j]->topicname, clnt_sub[k]->topicname) == 0)
                            clnt_sub[k]->flag = true;
                    }
                }


                if(--nready <= 0)
                    break;

            }

            if(FD_ISSET(sockfd, &wset) )
            {
                for(j=0; j<clnt_sub_idx; j++)
                {
                    //읽을 소켓에 대한 subscribe를 가져옴
                    if(sockfd != clnt_sub[j]->sock_fd)
                        continue;


                    for(k=0; k<topic_idx; k++)
                    {
                        if(strcmp(clnt_sub[j]->topicname, topic_arr[k]->name) != 0)
                            continue;

                        if(clnt_sub[j]->flag)
                        {
                            write(sockfd, topic_arr[k]->message, BUFFER_SIZE);

                            clnt_sub[j]->flag = false;
                        }
                    }
                }

                if(--nready <= 0)
                    break;
            }
        }
    }

    close(listenfd);
    return 0;
}
