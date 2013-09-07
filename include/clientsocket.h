#ifndef _CLIENTSOCKET_H
#define _CLIENTSOCKET_H

#include <linkedlist.h>

#define CLISOCK_BUFSIZE 4096
//#define CLISOCK_BUFSIZE 4

typedef struct _ClientSocket {

    int fd;

    char buf[CLISOCK_BUFSIZE];
    int curRead;

    int closed;

} ClientSocket;

ClientSocket *new_ClientSocket(int );
void DeleteClientSocket(ClientSocket * clisock);
int isClosed(ClientSocket *);
int isBufferFull(ClientSocket *);
int isBufferEmpty(ClientSocket *);
void closeSocket(ClientSocket *);


void handleread(ClientSocket *);
void handlewrite(ClientSocket *);


#endif // for #ifndef _CLIENTSOCKET_H
