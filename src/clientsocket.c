#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <clientsocket.h>
#include <logger.h>

ClientSocket *new_ClientSocket(int fd) {
    ClientSocket *clisock = malloc(sizeof(ClientSocket) );

    if ( ! clisock)
        return NULL;

    clisock->fd = fd;
    clisock->curRead = 0;
    clisock->closed = 0;

    return clisock;
}
void DeleteClientSocket(ClientSocket * clisock) {
    free(clisock);
}

int isClosed(ClientSocket *clisock) {
    return clisock->closed;
}
int isBufferFull(ClientSocket *clisock) {
    return clisock->curRead == CLISOCK_BUFSIZE;
}
int isBufferEmpty(ClientSocket *clisock) {
    return clisock->curRead == 0;
}
void closeSocket(ClientSocket *clisock) {
    clisock->closed = 1;
}


void handleread(ClientSocket *clisock) {
    int n;
    n = recv( clisock->fd
              , & clisock->buf[clisock->curRead]
              , CLISOCK_BUFSIZE - clisock->curRead
              , 0
            );

    if(n < 0) {
        if (errno == EINTR) {
            logger(LOG_WARN, "recv() EINTR. Try again later.");
            return;
        }
        logger(LOG_WARN, "recv() Error: %s", strerror(errno));
        clisock->closed = 1;
    }
    else if(n == 0) {
        logger(LOG_DEBUG, "Client connection closed (fd: %d)", clisock->fd);
        clisock->closed = 1;
    }
    else {
        logger(LOG_DEBUG, "Read %d bytes from client(fd: %d)", n, clisock->fd);
        clisock->curRead += n;
    }

}

void handlewrite(ClientSocket *clisock) {
    int n;
    n = send(clisock->fd
             , & clisock->buf[0]
             , clisock->curRead
             , 0
            );

    if(n < 0) {
        if (errno == EINTR) {
            logger(LOG_WARN, "send() EINTR. Try again later.");
            return;
        }
        logger(LOG_WARN, "send() Error: %s", strerror(errno));
        clisock->closed = 1;
    }
    else if(n != clisock->curRead) {
        logger(LOG_WARN, "Can't send whole buffer to client (fd: %d)", clisock->fd);
        clisock->closed = 1;
    }
    else {
        logger(LOG_DEBUG, "Send %d bytes to client(fd: %d)", n, clisock->fd);
        clisock->curRead = 0;
    }
}

