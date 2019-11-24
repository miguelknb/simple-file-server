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

	msg = msg_formatter();

	msg_controller(msg, rd_req, wr_req, fi_req, dir_req);

	free(wr_req);
	free(rd_req);
	free(fi_req);
	free(dir_req);

	return 0;
}