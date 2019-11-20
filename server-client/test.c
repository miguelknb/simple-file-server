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


char * msg_formatter( void ) {
	/*
	 * 0 = Read
	 * 1 = Write
	 * 2 = File
)	 * 3 = Direcrtory 
	 */

	char * msg = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
	char * path = (char*)malloc(MAXPATHLEN * sizeof(char));
	char * type = (char*)malloc(6 * sizeof(char));
	char * payload = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));

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
			fgets(payload, MAX_PAYLOAD_SIZE, stdin);
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
			
			break;
		}

		case 'D' : {
			break;
		}
	}

	printf("MSG: %s\n", msg);

	free(path);
	free(type);

	return msg;
}



int main(void) {

    RDreq * rd_req = (RDreq*)malloc(sizeof(RDreq));
    WRreq * wr_req = (WRreq*)malloc(sizeof(WRreq));
    FIreq * fi_req = (FIreq*)malloc(sizeof(FIreq));
    DIRreq * dir_req = (DIRreq*)malloc(sizeof(DIRreq));

	char * msg;
	printf("USAGE = WR-REQ*<path>*<payload>*<nrbytes>*<offset>*<client_id>*<ow_p>*<ot_p>\n");

	msg = msg_formatter();

	msg_controller(msg, rd_req, wr_req, fi_req, dir_req);

	// printf("path: %s\n", wr_req->path);
	// printf("offset: %d\n", wr_req->offset);
	// printf("nrbytes: %d\n", wr_req->nrbytes);
	// printf("payload: %s\n", wr_req->payload);
	// printf("client_id: %d\n", wr_req->client_id);
	// printf("owner_perm: %c\n", wr_req->owner_perm);
	// printf("others_perm: %c\n", wr_req->other_perm);
	// printf("\n\n");


	free(wr_req);
	free(rd_req);
	free(fi_req);
	free(dir_req);

}