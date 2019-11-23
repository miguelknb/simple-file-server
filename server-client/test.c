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

int main(void) {

    RDreq * rd_req = (RDreq*)malloc(sizeof(RDreq));
    WRreq * wr_req = (WRreq*)malloc(sizeof(WRreq));
    FIreq * fi_req = (FIreq*)malloc(sizeof(FIreq));
    DIRreq * dir_req = (DIRreq*)malloc(sizeof(DIRreq));

	char * msg;
	printf("USAGE = WR-REQ*<path>*<payload>*<nrbytes>*<offset>*<client_id>*<ow_p>*<ot_p>\n");

	msg = msg_formatter();

	msg_controller(msg, rd_req, wr_req, fi_req, dir_req);

	printf("id = %d\n", fi_req->owner);
	printf("owp: %c\n", fi_req->owner_permission);
	printf("otp: %c\n", fi_req->others_permission);
	printf("size: %ld\n", fi_req->file_length);

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