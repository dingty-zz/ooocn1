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
    clisock->readIndex = 0;
    clisock->closed = 0;

    if ( init_request(& clisock->request) < 0 ) {
        free(clisock);
        return NULL;
    }

    init_response( & clisock->response);

    return clisock;
}
void DeleteClientSocket(ClientSocket * clisock) {
    delete_request(& clisock->request);
    delete_response(& clisock->response);
    free(clisock);
}

int ableToRead(ClientSocket *clisock) {
    return ( clisock->request.state != REQ_DONE
             && clisock->readIndex < CLISOCK_BUFSIZE );
}
int ableToWrite(ClientSocket *clisock) {
    return clisock->request.state == REQ_DONE;
}

/*
@brief
  This functions provides the interface for how to handle read bytes on
  specific client socket.
 */
void handleread(ClientSocket *clisock) {
    int n;
    int ctLength;

    if(clisock->readIndex == CLISOCK_BUFSIZE)
        return;

    switch (clisock->request.state) {
    case REQ_CONTENT:
        ctLength = clisock->request.ctLength;
        if( ctLength <= clisock->readIndex)
            return;
        n = recv(clisock->fd,
                 & clisock->readbuf[clisock->readIndex],
                 ctLength - clisock->readIndex,
                 0 );
        break;
    case REQ_LINE:
    case REQ_HEADER:
        // get one byte at a time for easier parsing
        n = recv(clisock->fd,
                 & clisock->readbuf[clisock->readIndex],
                 1,
                 0 );
        break;
    default:
        return;
    }

    if(n < 0) {
        if (errno == EINTR) {
            logger(LOG_WARN, "recv() EINTR. Try again later.");
            return;
        }
        logger(LOG_ERROR, "recv() Error: %s", strerror(errno));
        clisock->closed = 2; //close socket
    }
    else if(n == 0) {
        logger(LOG_DEBUG, "Client connection closed (fd: %d)", clisock->fd);
        clisock->closed = 2; //close socket
    }
    else {
        logger(LOG_DEBUG, "Read %d bytes from client(fd: %d)", n, clisock->fd);
        clisock->readIndex += n;
    }

}

/*
@brief
  This functions provides the interface for how to handle send bytes on
  specific client socket.
 */
void handlewrite(ClientSocket *clisock) {
    int n;

    n = send(clisock->fd
             , clisock->writebuf
             , clisock->writeIndex
             , 0
            );

    if(n < 0) {
        if (errno == EINTR) {
            logger(LOG_WARN, "send() EINTR. Try again later.");
            return;
        }
        logger(LOG_ERROR, "send() Error: %s", strerror(errno));
        clisock->closed = 2; //close socket
    }
    else if(n != clisock->writeIndex) {
        logger(LOG_WARN, "Can't send whole buffer to client (fd: %d)", clisock->fd);
        clisock->closed = 2; //close socket
    }
    else {
        logger(LOG_INFO, "Send %d bytes to client(fd: %d)", n, clisock->fd);
        clisock->writeIndex = 0;
    }

    if(clisock->response.state == -1){
        clisock->closed = 1;
    }

}

