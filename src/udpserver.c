/* UDP SERVER*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "filesystem.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>

#define _GNU_SOURCE
#define PAGESIZE 4096

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char **argv)
{

	/* udp variables */
	int sockfd;					   /* socket */
	int portno;					   /* port to listen on */
	int clientlen;				   /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp;		   /* client host info */
	char buf[BUFSIZE];			   /* message buf */
	char *hostaddrp;			   /* dotted decimal host addr string */
	int optval;					   /* flag value for setsockopt */
	int n;						   /* message byte size */

	/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
	/* File Server Variables */

	RDreq * rd_req = (RDreq*)malloc(sizeof(RDreq));
    WRreq * wr_req = (WRreq*)malloc(sizeof(WRreq));
    FIreq * fi_req = (FIreq*)malloc(sizeof(FIreq));
    DIRreq * dir_req = (DIRreq*)malloc(sizeof(DIRreq));

	char msg[BUFSIZE];
	char * reply = (char*)malloc(BUFSIZE * sizeof(char));

	/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
	
	/* check command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	portno = atoi(argv[1]);

	/* 
   * socket: create the parent socket 
   */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	/* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

	/*build the server's Internet address*/
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);

	/* bind: associate the parent socket with a port */
	if (bind(sockfd, (struct sockaddr *)&serveraddr,
			 sizeof(serveraddr)) < 0)
		error("ERROR on binding");

	/* main loop: wait for a command, then execute it */
	clientlen = sizeof(clientaddr);

	while (1)
	{

	/* recvfrom: receive a UDP datagram from a client */
		bzero(msg, BUFSIZE);
		n = recvfrom(sockfd, msg, BUFSIZE, 0, (struct sockaddr *)&clientaddr, &clientlen);

		if (n < 0)
			error("ERROR in recvfrom");

	/* gethostbyaddr: determine who sent the datagram */

		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);

		if (hostp == NULL) error("ERROR on gethostbyaddr");

		hostaddrp = inet_ntoa(clientaddr.sin_addr);

		if (hostaddrp == NULL) error("ERROR on inet_ntoa\n");

		printf("\n> Message received: %s\n", msg);
	/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

	reply = msg_controller(msg, rd_req, wr_req, fi_req, dir_req);
		
	/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

	/* sendto: echo the response back to the client */
		n = sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&clientaddr, clientlen);
		if (n < 0)
			error("ERROR in sendto");
	}
}