/* filesystem.h */


/* Struct with read request parameters*/

typedef struct rd_req {
    char * path;
    int len;
    char * payload;
    int nrbytes;
    int offset;
    int client_id;
}RDreq;

/* Struct with write request parameters*/

typedef struct wr_req {
    char * path;
    int len;
    char * payload;
    int nrbytes;
    int offset;
    int client_id;
}WRreq;

/* Struct with file(info) request parameters*/

typedef struct fi_req {
    char * path;
    int len;
}FIreq;


/* Struct with directory(create, list, remove) request parameters*/

typedef struct dir_req {
    char * path;
    int len;
    char * dirname;
    int dirnamelen;
    int client_id;
}DIRreq;


/* Functions */

int write_parser(char * msg, WRreq * req);
/* Write request string parser. Pass parameters through WRreq */ 

int read_parser(char * msg, RDreq * req);
/* Read request string parser. Pass parameters through RDreq */ 

int file_parser(char * msg, FIreq * req);
/* File request string parser. Pass parameters through FIreq */ 

int dir_parser(char * msg, DIRreq * req);
/* Directory request string parser. Pass parameters through DIRreq */ 

int msg_parser(char * msg, RDreq * rd_req, WRreq * wr_req, FIreq * fi_req, DIRreq * dir_req);
/* Generic msg string parser. Sends string to it's respective parser */

