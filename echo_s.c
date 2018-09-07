#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define LISTEN_BACKLOG 1
#define PORT 9872

int processConnection(int connfd) 
{

	char buffer[1024]; //make 1k bits the max i suppose

	ssize_t read_size;

	while ( (read_size = recv(connfd, buffer, 1024, 0)) > 0)
	{
		printf("reading %ld bytes\n", read_size);

		if (strncmp("CLOSE", buffer, 5) == 0)
		{
			puts("lets close this shiz");
			return 0;
		}
		write(connfd, buffer, strlen(buffer));

	}


	if (read_size == 0) 
	{
		puts("client disconnected");
		printf("read failed %s\n", strerror(errno));
		return -1;
	}

	printf("data acknowledged received %ld bytes\n", read_size);

	return 0;
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



		int connfd = -1;
		if ((connfd = accept(lfd, (struct sockaddr *) NULL, NULL)) < 0) 
		{
			printf("acceptfailed %s\n", strerror(errno));
		}

		int ret = processConnection(connfd);

		close(connfd);

		printf("accept success\n");

	return 0;
}













