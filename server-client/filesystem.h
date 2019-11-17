/* filesystem.h */

#define MAX_PAYLOAD_SIZE 512

#define OFFSET 20
/*
 * Data regarding file owner and permissions to other users
 * are kept in the first 20 bytes of the file. So, the read
 * offset 0 actually begins on the 20th byte. 
 * 
 */

/* Struct read parameters */

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
    char owner_perm;
    char other_perm;
}WRreq;

/* Struct with file(info) request parameters*/

typedef struct fi_req {
    char * path;
    int len;
    int owner;
    char permissions;
    long int file_length;

}FIreq;


/* Struct with directory(create, list, remove) request parameters*/

typedef struct dir_req {
    char * path;
    int len;
    char * dirname;
    int dirnamelen;
    int client_id;
}DIRreq;



/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* Parsing functions */

int msg_controller(char * msg, RDreq * rd_req, WRreq * wr_req, FIreq * fi_req, DIRreq * dir_req);
/* Generic msg string parser. Sends string to it's respective parser
 * 
 */


int write_parser(char * msg, WRreq * req);
/* Write request string parser. Pass parameters through WRreq */ 

int read_parser(char * msg, RDreq * req);
/* Read request string parser. Pass parameters through RDreq */ 

int file_parser(char * msg, FIreq * req);
/* File request string parser. Pass parameters through FIreq */ 

int dir_parser(char * msg, DIRreq * req);
/* Directory request string parser. Pass parameters through DIRreq */ 


/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* File System Functions */

int file_info(FIreq * fi_req);

int file_read(RDreq * rd_req);

int directory_create(DIRreq * dir_req);

int file_write(WRreq * wr_req);