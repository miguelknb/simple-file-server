#include "filesystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>



int msg_controller(char * msg, RDreq * rd_req, WRreq * wr_req, FIreq * fi_req, DIRreq * dir_req) {

    if (msg[0] == 'R') {
        printf("READ\n");
        if ( read_parser(msg, rd_req) < 0 ) {
            rd_req->payload = "Error during parsing";
        }

        if ( file_read(rd_req) < 0) {
            rd_req->payload = "Error opening file";
        }
        printf("Parse complete\n");
        return 0;

    }
    else if (msg[0] == 'W') {
        printf("READ\n");
        if ( write_parser(msg, wr_req) < 0 ) {
            rd_req->payload = "Error during parsing";
        }

        if ( file_write(wr_req) < 0) {
            rd_req->payload = "Error opening file";
        }
        printf("Parse complete\n");
        return 0;
    }
    else if (msg[0] == 'F') {
        printf("FILE\n");
        file_parser(msg, fi_req);
        printf("Parse complete\n");
        return 0;
    }   
    else if (msg[0] == 'D') {
        printf("DIRECTORY\n");
        dir_parser(msg, dir_req);
        printf("Parse complete\n");
        return 0;
    }

    else 
        return -1;

    return 0;
}

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* Parse Functions */


int write_parser(char * msg, WRreq * req) {
    int i = 0;
    int size = strlen(msg);
    char tempMsg[size];
    const char s[2] = "*";
    char * token;
    char params[8][64];

    strcpy(tempMsg, msg);
    token = strtok(tempMsg, s);    

    while( token != NULL ) {
        strcpy(params[i], token);
        token = strtok(NULL, s);
        i++;
   }


    req->path = (char*)malloc(sizeof(char)*strlen(params[1]));
    req->payload = (char*)malloc(sizeof(char)* MAX_PAYLOAD_SIZE);

    strcpy(req->path, params[1]);
    strcpy(req->payload, params[2]);
    req->len = sizeof(req->path);
    req->nrbytes = atoi(params[3]);
    req->offset = atoi(params[4]);
    req->client_id = atoi(params[5]);
    req->owner_perm = params[6][0];
    req->other_perm = params[7][0];

    return 0;
}

int read_parser(char * msg, RDreq * req) {
    int i = 0;
    int size = strlen(msg);
    char tempMsg[size];
    const char s[2] = "*";
    char * token;
    char params[7][64];

    strcpy(tempMsg, msg);
    token = strtok(tempMsg, s);    

    while( token != NULL ) {
        strcpy(params[i], token);
        token = strtok(NULL, s);
        i++;
   }

    req->path = (char*)malloc(sizeof(char)*strlen(params[1]));
    req->payload = (char*)malloc(sizeof(char)* MAX_PAYLOAD_SIZE);


    strcpy(req->path, params[1]);
    req->len = sizeof(req->path);
    req->nrbytes = atoi(params[2]);
    req->offset = atoi(params[3]);
    req->client_id = atoi(params[4]);

   return 0;
}

int file_parser(char * msg, FIreq * req) {
    int i = 0;
    int size = strlen(msg);
    char tempMsg[size];
    const char s[2] = "*";
    char * token;
    char params[2][64];

    strcpy(tempMsg, msg);
    token = strtok(tempMsg, s);    

    while( token != NULL ) {
        strcpy(params[i], token);
        token = strtok(NULL, s);
        i++;
   }

    req->path = (char*)malloc(sizeof(char)*strlen(params[1]));

    strcpy(req->path, params[1]);
    req->len = sizeof(req->path);

    return 0;
}

int dir_parser(char * msg, DIRreq * req) {
    int i = 0;
    int size = strlen(msg);
    char tempMsg[size];
    const char s[2] = "*";
    char * token;
    char params[7][64];

    strcpy(tempMsg, msg);
    token = strtok(tempMsg, s);    

    while( token != NULL ) {
        strcpy(params[i], token);
        token = strtok(NULL, s);
        i++;
   }

    req->path = (char*)malloc(sizeof(char)*strlen(params[1]));
    req->dirname = (char*)malloc(sizeof(char)*strlen(params[3]));

    strcpy(req->path, params[1]);
    strcpy(req->dirname, params[3]);
    req->len = sizeof(req->path);
    req->dirnamelen = sizeof(req->dirname);
    req->client_id = atoi(params[5]);

    return 0;
}

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* File System Functions */

static char * metadata_insert(WRreq * wr_req) {
    int count;
    char * owner_id = (char*)malloc(12*sizeof(char));
    /* <id>\n */
    char * owner_perm = (char*)malloc(2*sizeof(char));
    /* <w/d>\n */ 
    char * others_perm = (char*)malloc(2*sizeof(char));
    /* <w/d>\n */

    char * metadata = (char*)malloc(20*sizeof(char));
    /* template:
     *
     * <owner_id>
     * <owner_permission>
     * <others_permission>
     * <complete_bytes>
     * 
     */

    sprintf(owner_id, "%d" ,wr_req->client_id);
    strcat(owner_id, "\n");
    strcat(metadata, owner_id);
    owner_perm[0] = wr_req->owner_perm;
    owner_perm[1] = '\n';
    strcat(metadata, owner_perm);
    others_perm[0] = wr_req->other_perm;
    others_perm[1] = '\n';
    strcat(metadata, others_perm);
    


    return metadata;
}


int file_read(RDreq * rd_req) {
	int fd, i = 0;
	char path[256] = "../SFS-root-dir";
	char * p;
	struct stat buffer;
	char * payload = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));

    int count_bytes = 0;
    int total_bytes = rd_req->nrbytes;
    int current;

	strcat(path, rd_req->path);

	fd = open(path, O_RDONLY);

	if(fd == -1){
		return 1;
	}

	if(fstat(fd, &buffer) == -1) {
		return 1;
	}

	p = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED){
		return 1;
	}

    current = OFFSET + rd_req->offset; 
    while(count_bytes < total_bytes) {
        payload[i] = p[current];
        count_bytes++;
        current++;
        i++;
    }

	if(munmap(p, buffer.st_size) == -1){
		return 1;
	}

    payload = (char*)realloc(payload, sizeof(char)*total_bytes);

	rd_req->payload = payload;

	return 0;
}

int directory_create(DIRreq * dir_req) {
    
    return 0;
}

int file_write(WRreq * wr_req) {
    char * metadata = metadata_insert(wr_req);
    int fd, i = 0;
	char path[256] = "../SFS-root-dir";
	char * p;
	struct stat buffer;

    int count_bytes = 0;
    int total_bytes = wr_req->nrbytes;
    int current;

    char * payload = wr_req->payload;

	strcat(path, wr_req->path);

    

    if (access(path, F_OK) != -1 ) {
        // file exists

        // fd = open(path, O_RDONLY);

        // if(fd == -1){
        //     return 1;
        // }

        // if(fstat(fd, &buffer) == -1) {
        //     return 1;
        // }

        // p = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);

        // if(p == MAP_FAILED){
        //     return 1;
        // }


    }
    
    else {
        //file doesn't exists, inserting metadata
    }

    printf("METADATA:\n%s\n", metadata);

    return 0;
}