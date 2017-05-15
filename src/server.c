#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CONNECT_RETRY (5)

#define PORT_NO (17000)

int main(int argc, char* argv[])
{
	int s32i;
	int s32ret;
	int s32listenfd;
	int s32newskfd;
	int s32optval;
	socklen_t s32optlen;
	socklen_t s32len;
	int32_t s32temp;
	struct sockaddr_in straddr;


	/* Create Socket FD */
	s32listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s32listenfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	bzero((char *) &straddr, sizeof(straddr));
	straddr.sin_family      = AF_INET;
	straddr.sin_addr.s_addr = INADDR_ANY;
	straddr.sin_port        = htons(PORT_NO);

	/* Avoid Port already in use problem */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32listenfd, SOL_SOCKET, SO_REUSEADDR, &s32optval, s32optlen);

	/* Using a long-live TCP connection */
	s32optval = 1;
	s32optlen = sizeof(int);
	setsockopt(s32listenfd, SOL_SOCKET, SO_KEEPALIVE, &s32optval, s32optlen);

	/* Bind the serv_addr + port with the dedicate socket */
	if (bind(s32listenfd, (struct sockaddr *) &straddr,
		  sizeof(straddr)) < 0) 
	{
		perror("ERROR on binding");
		return -1;
	}
	/* Listen and setup the outstanding connect request */
	listen(s32listenfd, 5);


	while(1)
	{
		s32len = sizeof(straddr);
		for(s32i = 0; s32i < CONNECT_RETRY; s32i++)
		{
			s32newskfd = accept(s32listenfd, 
							   (struct sockaddr *)&straddr,
							   &s32len);
			if(s32newskfd < 0)
			{
				perror("ERROR Connection socket: Retry!");
				continue;
			}
			else
			{
				break;
			}
		}
		if(s32newskfd < 0)
		{
			perror("ERROR: Retry Time over!");
			exit (EXIT_FAILURE);
		}

		printf("Connected!\n");

		while(1)
		{
			s32ret = recv(s32newskfd, &s32temp, sizeof(int32_t), MSG_WAITALL);
			if (s32ret != sizeof(int32_t)) 
			{		
				perror("ERROR read");
				exit (EXIT_FAILURE);
			}
			/* printf("Rcv[%d]\n", s32temp); */
			if(s32temp < 0)
			{
				/* When Recv a negative intergeri, close current connection */
				break;
			}
			s32ret = send(s32newskfd, &s32temp, sizeof(int32_t), MSG_DONTWAIT);
			if (s32ret != sizeof(int32_t)) 
			{		
				perror("ERROR write");
				exit (EXIT_FAILURE);
			}
		}

		close(s32newskfd);
		printf("Disonnected!\n");
	}

	close(s32listenfd);

	return 0;
}
