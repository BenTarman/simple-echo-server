#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#define LISTEN_BACKLOG 1

#define KRED   "\x1b[31m"
#define KYEL   "\x1b[33m"
#define KGREEN "\x1b[32m"
#define KBLUE  "\x1b[34m"
#define KRESET "\x1b[0m"
	
int isVerbose = 0; //global variable for  verbose mode

void debug(char* format, ...) 
{
	if (isVerbose) 
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}

int processConnection(int connfd) 
{

	char buffer[256]; //make 1k bits the max i suppose
	ssize_t read_size;

	//clear buffer (prevents overflows)
	bzero(buffer,256);
	debug(KYEL "calling read..." KRESET "\n");
	while ( (read_size = recv(connfd, buffer, 256, 0)) > 0)
	{
		buffer[strcspn(buffer, "\n\r")] = '\0'; //clears newline from buffer

		debug("Received %ld bytes, containing the string \"%s\"\n", read_size, buffer);

		if (strncmp("CLOSE", buffer, 5) == 0)
		{
			debug(KBLUE "Close command found" KRESET "\n");
			return 0;
		}
		
		if (strncmp("QUIT", buffer, 4) == 0) 
		{
			debug(KBLUE "QUIT command found" KRESET "\n");
			return 1;
		}


		debug(KYEL "calling write..." KRESET "\n");
		write(connfd, buffer, strlen(buffer));
		write(connfd, "\n", strlen("\n"));
		debug(KGREEN "Wrote %ld back to client" KRESET "\n", read_size);
		debug(KYEL "calling read..." KRESET "\n");
	}


	if (read_size == 0) 
	{
		puts("client disconnected");
		debug("read failed %s\n", strerror(errno));
		return -1;
	}

	debug("data acknowledged received %ld bytes\n", read_size);

	return 0;
}

int main(int argc, char **argv) 
{
  int opt = 0;


	// check if verbose mode enabled
  while ((opt = getopt(argc,argv,"v")) != -1) {
    
    switch (opt) {
    case 'v':
			isVerbose = 1;
      break;
    case ':':
    case '?':
    default:
			debug("usage: %s -v\n", argv[0]);
      exit(-1);
    }
  }


	// start the actual socket programming
	
	int lfd = -1;
	struct sockaddr_in my_addr;
	
	debug( KYEL "creating socket...." KRESET "\n");

	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		debug( KRED "Failed to create listening socket" KRESET "\n");
		exit(-1);
	}

	debug( KGREEN "created socket success (assigned file descriptor %d)" KRESET "\n", lfd);

	// assign random port number
	srand (time(NULL));
	int port = (rand() % (10000 - 1 + 1)) + 1; 
	char port_str[20];
	sprintf(port_str, "Using port %d", port);
	puts(port_str);

	//clear whole structure
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = PF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(port);


	debug( KYEL "binding socket...." KRESET "\n");

	if (bind(lfd, (struct sockaddr *) &my_addr, 
				sizeof(struct sockaddr_in)) == -1) 
	{
		debug(KRED "bind socket failed %s" KRESET "\n", strerror(errno));
	}

	debug( KGREEN "bind socket success" KRESET "\n");
	
	debug( KYEL "listen on socket..." KRESET "\n");

	if (listen(lfd, LISTEN_BACKLOG) == -1) 
	{
		debug( KRED "listen failed %s" KRESET "\n", strerror(errno));
	}
	
	debug( KGREEN "socket now listening..." KRESET "\n");


	int quitProgram = 0;
	while (!quitProgram) 
	{
		int connfd = -1;
		debug( KYEL "accepting socket...." KRESET "\n");
		if ((connfd = accept(lfd, (struct sockaddr *) NULL, NULL)) < 0) 
		{
			debug( KRED "acceptfailed %s" KRESET "\n", strerror(errno));
		}
		
		debug( KGREEN "socket accepted (connected on %d)" KRESET "\n", connfd);
		quitProgram = processConnection(connfd);
		close(connfd);
		debug( KBLUE "CONNECTION CLOSED" KRESET "\n");
	}
		
	debug( KBLUE "PROGRAM QUITTED" KRESET "\n");


	return 0;
}













