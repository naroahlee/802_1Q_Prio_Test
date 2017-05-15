#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

#define HOST_IP "10.0.0.11"
//#define HOST_IP "192.168.2.100"
/*#define HOST_IP "127.0.0.1" */
#define PORT_NO (17000)

int main(int argc, char* argv[])
{
	int s32ret;
	int s32rdfd;
	int32_t s32temp;
	struct sockaddr_in straddr;
    struct hostent *server;

	int s32i;
	int s32cnt;

	int s32prio;


	if(3 != argc)
	{
		fprintf(stderr, "Error Arg! Useage: ./client [TestSample] [802.1Q Prio]\n");
		exit (EXIT_FAILURE);
	}
	s32cnt   = atoi(argv[1]);
	s32prio  = atoi(argv[2]);

	/* Create Socket FD */
	s32rdfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s32rdfd < 0) 
	{		
		perror("ERROR opening socket");
		exit (EXIT_FAILURE);
	}

    server = gethostbyname(HOST_IP);
    if (server == NULL) 
	{
        perror("ERROR, no such host.");
		exit (EXIT_FAILURE);
    }

	/* Bind a name to the socket. */
    bzero((char *) &straddr, sizeof(straddr));
    straddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&straddr.sin_addr.s_addr,
          server->h_length);
    straddr.sin_port = htons(PORT_NO);


	/* Set PCP CoS in Layer-2 */
	if(setsockopt(s32rdfd, SOL_SOCKET, SO_PRIORITY, &s32prio, sizeof(s32prio)) < 0)
	{   
		printf("Set PCP 802.1Q Faied\n");
	}

    if (connect(s32rdfd, (struct sockaddr *)&straddr,sizeof(straddr)) < 0) 
	{
        perror("ERROR connecting");
		exit (EXIT_FAILURE);
	}

	printf("Connected!\n");
	/* '\0' matters */
	s32temp = 0x55AA55AA;
	for(s32i = 0; s32i < s32cnt; s32i++)
	{
		s32ret = send(s32rdfd, &s32temp, sizeof(int32_t), MSG_DONTWAIT);
		if (s32ret != sizeof(int32_t)) 
		{		
			perror("ERROR send");
			exit (EXIT_FAILURE);
		}
		s32ret = recv(s32rdfd, &s32temp, sizeof(int32_t), MSG_WAITALL);

		if (s32ret != sizeof(int32_t)) 
		{		
			perror("ERROR recv");
			exit (EXIT_FAILURE);
		}
		sleep(1);
	}

	/* Send Exit */
	s32temp = -1;
	s32ret = send(s32rdfd, &s32temp, sizeof(int32_t), MSG_DONTWAIT);
	if (s32ret != sizeof(int32_t)) 
	{		
		perror("ERROR send");
		exit (EXIT_FAILURE);
	}

	printf("Disconnected!\n");

	close(s32rdfd);

	return 0;
}
