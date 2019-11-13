#include "filesystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 512

int write_parser(char * msg, WRreq * req) {
    int i = 0;
    int size = strlen(msg);
    char tempMsg[size];
    const char s[2] = "*";
    char * token;
    char params[5][64];

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

int msg_parser(char * msg, RDreq * rd_req, WRreq * wr_req, FIreq * fi_req, DIRreq * dir_req) {
    if (msg[0] == 'R') {
        printf("READ\n");
        read_parser(msg, rd_req);
        return 0;

    }
    else if (msg[0] == 'W') {
        printf("WRITE\n");
        write_parser(msg, wr_req);
        return 0;
    }
    else if (msg[0] == 'F') {
        printf("FILE\n");
        file_parser(msg, fi_req);
        return 0;
    }   
    else if (msg[0] == 'D') {
        printf("DIRECTORY\n");
        dir_parser(msg, dir_req);
        return 0;
    }

    else 
        return -1;

    return 0;
}
