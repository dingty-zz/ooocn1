#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <selectpool.h>
#include <logger.h>
#include "pool_internal.h"

#define BUFSIZE 4096

#define SWAPINT(i,j) {(i) = (i)^(j); (j) = (i)^(j); (i) = (i)^(j); }
#define MAX(i,j) ( ( (i) > (j) )? (i): (j) )

/*
@brief
  this is the initialization function for the select pool
@param
  pool: the select pool
  listenfd: the file descriptor of listening socket
*/
void init_pool(SelectPool *pool, int listenfd) {
    pool->listenfd = listenfd;

    init_linkedlist(&pool->clients);
}


/*
@brief
  refresh sockets ready to read and/or write in select pool
@param
  pool: the select pool
*/
void refresh_select(SelectPool *pool) {
    /*struct timeval tv;*/
    /*tv.tv_sec = 0;*/
    /*tv.tv_usec = 1000;*/

    // get the read_set and write_set ready for select
    prepare_select(pool);

    pool->nready = select(pool->maxfd + 1,
                          &pool->read_set,
                          &pool->write_set,
                          NULL, NULL);
                          /*NULL, &tv);*/

    if(pool->nready < 0) {
        if(errno == EINTR) {
            logger(LOG_INFO, "select func is interrupted by signal");
        }
        else {
            logger(LOG_ERROR, "select func Error: %s", strerror(errno));
        }
    }
    else if(pool->nready == 0) {
        logger(LOG_DEBUG, "select returns nothing");
    }
}

/*
@brief
  Accept new client connection.
@param
  pool: the select pool
*/
void accept_newclient(SelectPool *pool) {
    int connfd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    /*If listening descriptor ready, add new client to pool*/
    if (FD_ISSET(pool->listenfd, &pool->read_set)) {
        if((connfd =
                    accept(pool->listenfd, (struct sockaddr *)&addr, &addr_len))
                < 0
          ) {
            logger(LOG_ERROR, "Could not accept more clients");
            return;
        }

        if( add_client(pool, connfd, addr.sin_addr) < 0) {
            logger(LOG_WARN, "Can't accept client (fd: %d)", connfd);
        }
        else {
            logger(LOG_INFO, "Accepted new client (fd: %d), ip: %s",
              connfd, inet_ntoa(addr.sin_addr));
        }
    }
}

/*
@brief
  This functions enumerates the pool and remote sockets that have already
  been closed.
@param
  pool: the select pool
*/
void removeClosedSocket(SelectPool *pool) {
    ll_Node *cur, *next;
    ClientSocket *clisock;
    int fd;

    cur = ll_start(& pool->clients);

    while ( cur != ll_end(& pool->clients) ) {
        next = cur->next;
        {
            clisock = (ClientSocket *) cur->item;
            if( clisock->closed == 2) {
                // remove client
                ll_remove(&pool->clients, cur);
                free(cur);
                fd = clisock->fd;
                DeleteClientSocket(clisock);
                close(fd);
                logger(LOG_INFO, "Remove client (fd: %d)", fd);
            } else if( clisock->closed == 1) {
                //clear response
                delete_response(& clisock->response);
                init_response( & clisock->response);
                clisock->closed = 0;

            }
        }
        cur = next;
    }
}
