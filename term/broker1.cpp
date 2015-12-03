
int main()
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fdset rset, allset;
	char line[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port htnos(PORT);

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

	for(i=0; i<FD_SETSIZE; i++)
		client[i] = -1;

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	while(1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(listenfd, &rset))
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);

			for(i=0; i<FD_SETSIZE; i++)
			{
				if(client[i]<0)
				{
					client[i] = connfd;
					break;
				}
			}
	
			if(i==FD_SETSIZE)
			{
				printf("too many clients\n");
				exit(1);
			}

			FD_SET(connfd, &allset);
		
			if(connfd > maxfd)
				maxfd = connfd;

			if(i > maxi)
				maxi = i;

			if(--nready <= 0)
				continue;
		}

		for(i=0; i <= maxi; i++)
		{
			if((sockfd = client[i] < 0))
				continue;

			if(FD_ISSET(sockfd, &rset))
			{
				if((n=read(sockfd, line, MAXLINE) == 0))
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
					write(sockfd, line, n);

				if(--nready <= 0)
					break;

			}
		}
