#ifndef _CLIENTSOCKET_H
#define _CLIENTSOCKET_H

#include <linkedlist.h>
#include <httprequest.h>
#include <httpresponse.h>

#include <openssl/ssl.h>
#include <netinet/in.h>

#define CLISOCK_BUFSIZE 8192
//#define CLISOCK_BUFSIZE 4

typedef struct _ClientSocket {

    int fd;
    struct in_addr ip;

    char readbuf[CLISOCK_BUFSIZE];
    char writebuf[CLISOCK_BUFSIZE];
    int readIndex;
    int writeIndex;

    int closed;

    HttpRequest request;
    HttpResponse response;

    int stdin_pipe[2];
    int stdout_pipe[2];
    pid_t child_pid;
    int cgi_preprocessed;

    char **envp;

    int isHTTPS;
    SSL *ssl;

} ClientSocket;

ClientSocket *new_ClientSocket(int );
void DeleteClientSocket(ClientSocket * clisock);
int ableToRead(ClientSocket *);
int ableToWrite(ClientSocket *);


void handleread(ClientSocket *);
void handlewrite(ClientSocket *);


#endif // for #ifndef _CLIENTSOCKET_H
