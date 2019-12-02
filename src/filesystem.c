#include "filesystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _GNU_SOURCE


char * msg_controller(char * msg, RDreq * rd_req, WRreq * wr_req, FIreq * fi_req, DIRreq * dir_req) {
    int rep;
    char * feedback = (char*)malloc(MAX_PAYLOAD_SIZE*sizeof(char));

    printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    printf("\n> Server Received a new Request\n");

    /* Read Request --------------------------------------------------------------- */ 

    if (msg[0] == 'R') {
        printf("  - Read Request\n");
        if ( read_parser(msg, rd_req) < 0 ) {
            feedback = "Error during parsing";
            return feedback;
        }

        if ( (rep = file_read(rd_req)) != 0) {
            if ( rep == 5) {
                feedback = "File does not exists";
                return feedback;
            }
        }

        bzero(feedback, MAX_PAYLOAD_SIZE);

        strncpy(feedback, rd_req->payload, rd_req->nrbytes);

        printf("feedback: %s\n", feedback);
        return feedback;

    }
    /* Write Request --------------------------------------------------------------- */

    else if (msg[0] == 'W') {
        
        printf("  - Write Request\n");

        if ( write_parser(msg, wr_req) < 0 ) {
            feedback = "Error during parsing";
            return feedback;
        }

        if ( (rep = file_write(wr_req)) != 0) {
            if ( rep == 2) {
                feedback = "Unauthorized acees";
                printf("  - Unauthorized access\n");
                return feedback;
            }
        }
        printf("  - Request executed successfully\n");
        feedback = "Request executed successfully";
        return feedback;

        printf("> Request finished\n");

    }
    /* File Request --------------------------------------------------------------- */


    else if (msg[0] == 'F') {
        printf("  - File Info Reques\n");
        if ( file_parser(msg, fi_req) < 0 ) {
            feedback = "Error during parsing";
            return feedback;
        }

        if ( (rep = get_metadata(fi_req)) != 0) {

            if ( rep == 5) {
                feedback = "File does not exists";
                return feedback;
            }
        }

        
        bzero(feedback, MAX_PAYLOAD_SIZE);
        sprintf(feedback, "owner: %d\nsize: %ld\n", fi_req->owner, fi_req->file_length);
        printf("> Request finished\n");
        return feedback;

    }


    /* Read Request --------------------------------------------------------------- */

    else if (msg[0] == 'D') {

        if ( dir_parser(msg, dir_req) < 0 ) {
            rd_req->payload = "Error during parsing";
        }

		if (msg[1] == 'C') {
			printf("  - Directory Create Request\n");

			if ( (rep = directory_create(dir_req)) != 0) {
                if ( rep == 2) {
                    feedback = "Unauthorized acees";
                    printf("  - Unauthorized access\n");
                    return feedback;
                }
            }
            printf("  - Request executed successfully\n");
            feedback = "Request executed successfully";
            printf("> Request finished\n");

		}

		else if (msg[1] == 'R') {
			printf("  - Directory Remove Request\n");

            if ( (rep = directory_remove(dir_req)) != 0) {
                if ( rep == 2) {
                    feedback = "Unauthorized acees";
                    printf("  - Unauthorized access\n");
                    return feedback;
                }
            }
            printf("  - Request executed successfully\n");
            feedback = "Request executed successfully";
            printf("> Request finished\n");

		}

		else if (msg[1] == 'L') {
			printf("  - Directory List Request\n");

            if ( (rep = directory_list(dir_req)) != 0) {
                if ( rep == 2) {
                    feedback = "Unauthorized acess";
                    printf("  - Unauthorized access\n");
                    return feedback;
                }

                if (rep == 3) {
                    feedback = "No files in this directory";
                    printf("  - No files in this directory\n");
                    return feedback;                    
                }
            }
            printf("  - Request executed successfully\n");
            printf("> Request finished\n");

            bzero(feedback, MAX_PAYLOAD_SIZE);

            strncpy(feedback, dir_req->payload, strlen(dir_req->payload));

            return feedback;

		}

    }

    else {
        printf("Invalid Request\n");
        printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");

        return feedback;
    }

    printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");

    return feedback;
}

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* Parse Functions */

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
	char * dirname = (char*)malloc(64 * sizeof(char));

	int nrbytes, offset, client_id;
	char ow_p, ot_p;
   

	printf("\nInsert request type\n  > ");
	scanf("%s", type);
	getchar();
	printf("\nInsert request path\n  > ");
	scanf("%s", path);
	getchar(); 

	switch( type[0] ) {

		case 'R' : {

			printf("\nInsert number of bytes to be read (nrbytes)\n  > ");
			scanf("%d", &nrbytes);
			printf("\nInsert offset\n  > ");
			scanf("%d", &offset);
			printf("\nInsert client id\n  > ");
			scanf("%d", &client_id);

			sprintf(msg,"%s*%s*%d*%d*%d", type, path, nrbytes, offset, client_id);
			
			break;
		}

		case 'W' : {
			printf("\nType in your payload\n  > ");
			fgets(payload, MAX_PAYLOAD_SIZE, stdin);
			payload[strlen(payload)-1] = '\0';
			printf("\nInsert number of bytes to be read (your payload has %ld bytes)\n  > ", strlen(payload));
			scanf("%d", &nrbytes);
			printf("\nInsert offset\n  > ");
			scanf("%d", &offset);
			printf("\nInsert client id\n  > ");
			scanf("%d", &client_id);
			printf("\nInsert owner permission (if you are the owner)\n  > ");
			getchar();
			scanf("%c", &ow_p);
			printf("\nInser others users permission (if you are the owner)\n  > ");
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

			printf("\nInsert dirname\n  > ");
			scanf("%s", dirname);
			printf("\nInsert client id\n  > ");
			scanf("%d", &client_id);
			printf("\nInsert owner permission (if you are the owner)\n  > ");
			getchar();
			scanf("%c", &ow_p);
			printf("\nInsert other permission (if you are the owner)\n  > ");
			getchar();
			scanf("%c", &ot_p);

			sprintf(msg,"%s*%s*%s*%d*%c*%c",type, path, dirname, client_id, ow_p, ot_p);

			break;
		}
	}

	//printf("MSG: %s\n", msg);

	free(path);
	free(type);

	return msg;
}


int authorization(char * authpath, int req_id) {
    int fd, i = 0;
	char path[256] = "../SFS-root-dir";
	char * p;
	struct stat buffer;
	char * temp = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    const char s[2] = "\n";
    char * token;
    char params[4][4];
    char tempMsg[10];
    char own_p, oth_p;
    int j = 0;
    int id;

    strcat(path, authpath);
	fd = open(path, O_RDONLY);

	if(fd == -1){
		return 2;
	}

	if(fstat(fd, &buffer) == -1) {
		return 2;
	}

	p = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED){
		return 2;
	}

    while(i < 10) {
        temp[i] = p[i];
        i++;
    }

    strcpy(tempMsg, temp);
    token = strtok(tempMsg, s);

    while( token != NULL ) {
        strcpy(params[j], token);
        token = strtok(NULL, s);
        j++;
   }

    id = atoi(params[0]);
    own_p = params[1][0];
    oth_p = params[2][0];

    if ( id == req_id && own_p == 'w') {
        /* has permission */
        printf("     -> Permission granted\n");
        return 1;
    }

    if (oth_p == 'w' ) {
        /* has permission */
        printf("     -> Permission granted for user %d\n", id);
        return 1;
    }
    printf("     -> Permission denied for user %d\n", id);

    free(temp);
	return 0;
}


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
    char params[6][64];

    strcpy(tempMsg, msg);
    token = strtok(tempMsg, s);    

    while( token != NULL ) {
        strcpy(params[i], token);
        token = strtok(NULL, s);
        i++;
   }

    req->path = (char*)malloc(sizeof(char)*strlen(params[1]));
    req->dirname = (char*)malloc(sizeof(char)*strlen(params[2]));

    strcpy(req->path, params[1]);
    strcpy(req->dirname, params[2]);
    req->len = sizeof(req->path);
    req->dirnamelen = sizeof(req->dirname);
    req->client_id = atoi(params[3]);
	req->owner_permission = params[4][0];
	req->others_permission = params[5][0];

    return 0;
}

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/* File System Functions */

static char * metadata_insert(WRreq * wr_req) {
    int m_len;
    char * metadata = (char*)malloc(10*sizeof(char));
    /* template:
     *
     * <owner_id>
     * <owner_permission>
     * <others_permission>
     * <complete_bytes>
     * 
     */

    sprintf(metadata, "%d\n%c\n%c\n", wr_req->client_id, wr_req->owner_perm, wr_req->other_perm);
    
    m_len = strlen(metadata);

    while (m_len < 9) {
        strcat(metadata, "*");
        m_len++;
    }

    metadata[9] = '\n';

    return metadata;
}

int get_metadata(FIreq * fi_req) {
	int fd, i = 0;
	char path[256] = "../SFS-root-dir";
	char * p;
	struct stat buffer;
	char * temp = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    const char s[2] = "\n";
    char * token;
    char params[4][4];
    char tempMsg[10];
    char own_p, oth_p;
    int j = 0;
    int id;

    strcat(path, fi_req->path);

    if (access(path, F_OK) == -1 ) {
        printf("   * File does not exist\n");
        return 5;
    }

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

    while(i < 10) {
        temp[i] = p[i];
        i++;
    }

    strcpy(tempMsg, temp);
    token = strtok(tempMsg, s);

    while( token != NULL ) {
        strcpy(params[j], token);
        token = strtok(NULL, s);
        j++;
   }

    id = atoi(params[0]);
    own_p = params[1][0];
    oth_p = params[2][0];

    fi_req->owner = id;
    fi_req->owner_permission = own_p;
    fi_req->others_permission = oth_p;
	fi_req->file_length = buffer.st_size;

    free(temp);
    munmap(p, buffer.st_size);

    return 0;
}

int file_read(RDreq * rd_req) {
	int fd, i = 0;
	char path[256] = "../SFS-root-dir";
	char * p;
	struct stat buffer;
	char * payload = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    rd_req->payload = (char*)malloc(rd_req->nrbytes);

    int count_bytes = 0;
    int total_bytes = rd_req->nrbytes;
    int current;

	strcat(path, rd_req->path);

    if (access(path, F_OK) == -1 ) {
        printf("   * File does not exist\n");
        return 5;
    }

	fd = open(path, O_RDONLY);

	if(fd == -1){
		return 2;
	}

	if(fstat(fd, &buffer) == -1) {
		return 2;
	}

	p = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED){
		return 2;
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
    payload = (char*)realloc(payload,total_bytes);
	strcpy(rd_req->payload, payload);

	return 0;
}

int directory_create(DIRreq * dir_req) {
    int m_len;
    FILE * fp;
	char path[256] = "../SFS-root-dir";
    char * dirpath = (char*)malloc(MAXPATHLEN);
	char * metadata = (char*)malloc(10*sizeof(char));

	sprintf(metadata, "%d\n%c\n%c\n", dir_req->client_id, dir_req->owner_permission, dir_req->others_permission);

	m_len = strlen(metadata);

    while (m_len < 9) {
        strcat(metadata, "*");
        m_len++;
    }

    metadata[9] = '\n';
    printf("   * Creating directory\n");


    strcpy(dirpath, path);
	strcat(dirpath, dir_req->path);
	strcat(dirpath, dir_req->dirname);
	mkdir(dirpath, 0777);
	strcat(dirpath, "/.dir");

    printf("   *Creating .dir metadata\n");

	fp = fopen(dirpath,"w");

    fputs(metadata, fp);
    printf("   *Inserting metadata\n");

	fclose(fp);

    return 0;
}


int remove_directory(char * path) {
    int count,i;
    char *temppath = (char*)malloc(MAXPATHLEN);
    char *dirpath = (char*)malloc(MAXPATHLEN);
    struct stat archive;
    struct direct **files;
    int file_select(); 
    char * name;

    bzero(temppath, MAXPATHLEN);
    strcpy(temppath, path);
    strcpy(dirpath, path);
    count = scandir(dirpath, &files, file_select, alphasort);


    printf("   * current dir = %s\n", dirpath);

    if (count <= 0) {
        printf("   * empty dir\n");
        rmdir(dirpath);
        return 1;
    }

    printf("   *  entering dir\n");
    strcat(dirpath, "/");
    strcpy(temppath, dirpath);
    for ( i = 1; i < count + 1 ; ++i) {
        name = files[i-1]->d_name;
        strcpy(temppath, dirpath);
        strcat(temppath, name);


        printf("   * current file path = %s\n", temppath);
        if ( stat(temppath,&archive)  < 0) {
            //printf("deu merda\n");
            return 1;
        }

        printf("   * Trying to remove file %s\n",name );
        
        if ( remove(temppath) != 0) {
            strcat(temppath, name);
            //printf("file is dir\n");
            remove_directory(temppath);
        }
        printf("   * removed file %s\n", name);
        bzero(temppath, MAXPATHLEN);
    }
    remove(path);
    return 0;
}

int directory_remove(DIRreq * dir_req) {
    char * authpath = (char*)malloc(MAXPATHLEN);
    strcpy(authpath ,dir_req->path);
    strcat(authpath ,"/.dir");

    char * path = (char*)malloc(MAXPATHLEN);
    strcpy(path,"../SFS-root-dir");
    strcat(path, dir_req->path);

    if (! authorization(authpath, dir_req->client_id)) {
        return 2;
    }

    remove_directory(path);
    return 0;
}

int file_select(const struct direct * entry) {     
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))            
        return 0;     
    else           
        return 1; 
}

extern int alphasort();

int directory_list(DIRreq * dir_req) {
    int count,i;
	char path[256] = "../SFS-root-dir";
    char * temp = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    dir_req->payload = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    struct stat archive;
    struct direct **files;
    int file_select();  

    strcat(path, dir_req->path);

    printf("  - Listing files in Directory =\"%s\"\n",dir_req->path);

    count = scandir(path, &files, file_select, alphasort);
    /* If no files found, make a non-selectable menu item */

    if (count <= 0) {
        printf("  - No files in this directory\n");
        return 3;
    }
    bzero(temp, MAX_PAYLOAD_SIZE);
    for ( i = 1; i < count + 1 ; ++i) {

        if ( stat(path,&archive)  < 0)
            return 1;

        strcat(temp, "-> ");
        strcat(temp, files[i-1]->d_name);
        strcat(temp, "\n");
    }

    strncpy(dir_req->payload, temp, strlen(temp));

    free(temp);
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

        printf("   * writing on file\n");
        
        if( ! authorization(wr_req->path, wr_req->client_id ) ){
            return 2;
        }

        fd = open(path, O_RDWR | O_CREAT);

        if(fd == -1){
            perror("fd");
            printf("   * invalid path\n");
            return 5;
        }

        if(fstat(fd, &buffer) == -1) {
            perror("fstat");
            return 2;
        }

        size_t fsize = buffer.st_size;

        if( (wr_req->offset + wr_req->nrbytes + OFFSET) > fsize - 1) {
            /* allocating more memory to file */
            printf("   * extending file' end\n");
            fsize = wr_req->nrbytes + wr_req->offset + OFFSET;
            ftruncate(fd, fsize);
        }

        p = mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if(p == MAP_FAILED){
            perror("mmap");
            return 2;
        }

        for(int k = 0; k < 10; k ++) {
            p[k] = metadata[k];
        }

        i = 0;
        current = OFFSET + wr_req->offset;
        for(count_bytes = 0; count_bytes < total_bytes; count_bytes ++) {
            p[current] = payload[i];
            current++;
            i++;
        }

        if (msync(p, fsize, MS_SYNC) == -1) {
            perror("Could not sync the file to disk");
        }

        if (munmap(p, fsize) == -1) {
            close(fd);
            perror("Error un-mmapping the file");
            return 2;
        }

        close(fd);

    }
    
    else {
        //file doesn't exists, inserting metadata
        size_t size = 10 + wr_req->nrbytes + wr_req->offset;

        printf("   * creating new file\n");

        fd = open(path, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

        if(fd == -1){
            printf("   * invalid path\n");
            return 5;
        }

        ftruncate(fd, size);

        p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if(p == MAP_FAILED){
            perror("mmap");
            return 2;
        }

        /* Inserting metadata */ 

        for(int k = 0; k < 10; k ++) {
            p[k] = metadata[k];
        }

        i = 0;
        current = OFFSET + wr_req->offset;
        for(count_bytes = 0; count_bytes < total_bytes; count_bytes ++) {
            p[current] = payload[i];
            current++;
            i++;
        }

        if (msync(p, size, MS_SYNC) == -1) {
            perror("Could not sync the file to disk");
        }

        if (munmap(p, size) == -1) {
            close(fd);
            perror("Error un-mmapping the file");
            return 2;
        }

        close(fd);
    }

    return 0;
}
