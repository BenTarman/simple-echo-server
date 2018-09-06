#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LISTEN_BACKLOG 1
#define PORT 9873

int processConnection(int connfd) 
{



	return 1;
}

int main() 
{

	int lfd = -1;

	struct sockaddr_in my_addr;

	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		//handle_error("socket");
		printf("Failed to create listening socket");
	}

	printf("created socket\n");

	//clear whole structure
	memset(&my_addr, 0, sizeof(struct sockaddr_in));

	my_addr.sin_family = PF_INET;

	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(PORT);


	if (bind(lfd, (struct sockaddr *) &my_addr, 
				sizeof(struct sockaddr_in)) == -1) 
	{
		printf("bind failed %s\n", strerror(errno));
	}
	
	printf("bind socket\n");

	if (listen(lfd, LISTEN_BACKLOG) == -1) 
	{
		printf("listen failed %s\n", strerror(errno));
	}
	
	printf("listening on scoekt...\n");


	int quitProgram = 0;

	while (!quitProgram)
	{
		if ((connfd = accept(connfd, (struct sockaddr *) NULL, NULL)) < 0) 
		{
			printf("acceptfailed %s\n", strerror(errno));
		}

		quitProgram = processConnection(connfd)
//		createThreadAndProcess(connfd);

		close(connfd);


		printf("accept success %d\n", connfd);
	}

	return 0;
}













