/* UDP CLIENT*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

char * msg_formatter();

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char *buf = (char*)malloc(BUFSIZE * sizeof(char));

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    while(1) {

      /* get a message from the user */
      bzero(buf, BUFSIZE);
      buf = msg_formatter();

      /* send the message to the server */
      serverlen = sizeof(serveraddr);
      n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
      if (n < 0) 
        error("ERROR in sendto");
      
      /* print the server's reply */
      n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
      if (n < 0) 
        error("ERROR in recvfrom");
      printf("Response: %s", buf);
      
    }

    return 0;
}

char * msg_formatter( void ) {
	/*
	 * 0 = Read
	 * 1 = Write
	 * 2 = File
)	 * 3 = Direcrtory 
	 */

	char * msg = (char*)malloc(BUFSIZE * sizeof(char));
	char * path = (char*)malloc(BUFSIZE * sizeof(char));
	char * type = (char*)malloc(6 * sizeof(char));
	char * payload = (char*)malloc(BUFSIZE * sizeof(char));
	char * dirname = (char*)malloc(64 * sizeof(char));

	int nrbytes, offset, client_id;
	char ow_p, ot_p;


	printf("Request type: ");
	scanf("%s", type);
	getchar();  
	printf("Request path: " );
	scanf("%s", path);
	getchar(); 

	switch( type[0] ) {

		case 'R' : {

			printf("nrbytes: ");
			scanf("%d", &nrbytes);
			printf("offset: ");
			scanf("%d", &offset);
			printf("client id: ");
			scanf("%d", &client_id);

			sprintf(msg,"%s*%s*%d*%d*%d", type, path, nrbytes, offset, client_id);
			
			break;
		}

		case 'W' : {
			printf("payload: ");
			fgets(payload, BUFSIZE, stdin);
			payload[strlen(payload)-1] = '\0';
			printf("nrbytes (your payload has %ld bytes): ", strlen(payload));
			scanf("%d", &nrbytes);
			printf("offset: ");
			scanf("%d", &offset);
			printf("client id: ");
			scanf("%d", &client_id);
			printf("owner permission: ");
			getchar();
			scanf("%c", &ow_p);
			printf("other permission: ");
			getchar();
			scanf("%c", &ot_p);

			sprintf(msg,"%s*%s*%s*%d*%d*%d*%c*%c", type, path, payload, nrbytes, offset, client_id, ow_p, ot_p);
			break;
		}

		case 'F' : {
			sprintf(msg,"%s*%s",type, path);
			
			break;
		}

		case 'D' : {

			printf("dirname: ");
			scanf("%s", dirname);
			printf("client id: ");
			scanf("%d", &client_id);
			printf("owner permission: ");
			getchar();
			scanf("%c", &ow_p);
			printf("other permission: ");
			getchar();
			scanf("%c", &ot_p);

			sprintf(msg,"%s*%s*%s*%d*%c*%c",type, path, dirname, client_id, ow_p, ot_p);

			break;
		}
	}

	printf("MSG: %s\n", msg);

	free(path);
	free(type);

	return msg;
}