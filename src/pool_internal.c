#include <malloc.h>
#include <assert.h>
#include <fcntl.h>

#include <selectpool.h>
#include <clientsocket.h>
#include <logger.h>


void prepare_select(SelectPool *pool) {
    ClientSocket * clisock;
    ll_Node *iter;


    FD_ZERO(&pool->read_set);
    FD_ZERO(&pool->write_set);
    FD_SET(pool->listenfd, &pool->read_set);

    pool->maxfd = pool->listenfd;

    FOR_EACH_CLIENT(pool, iter, clisock) {
        assert(! isClosed(clisock));

        if ( ! isBufferFull(clisock)) {
            FD_SET(clisock->fd, & pool->read_set);
            if(clisock->fd > pool->maxfd)
                pool->maxfd = clisock->fd;
            logger(LOG_DEBUG, "add client (fd:%d) to read set", clisock->fd);
        }

        if ( ! isBufferEmpty(clisock) ) {
            FD_SET(clisock->fd, & pool->write_set);
            if(clisock->fd > pool->maxfd)
                pool->maxfd = clisock->fd;
            logger(LOG_DEBUG, "add client (fd:%d) to write set", clisock->fd);
        }
    }
}

int add_client(SelectPool *pool, int connfd) {
    ll_Node *clinode;
    ClientSocket * clisock;

    if( ll_count(pool->clients) == FD_SETSIZE )
        return -1;

    clisock = new_ClientSocket(connfd);

    if(!clisock)
        return -1;

    clinode = new_ll_Node(clisock);

    if(!clinode) {
        free(clisock);
        return -1;
    }

    ll_insert_last(pool->clients, clinode);

    return 0;

}

int set_nonblocking(int sockfd) {
    int opts;
    opts = fcntl(sockfd, F_GETFL);

    if (opts < 0)
        return -1;

    opts = (opts | O_NONBLOCK);

    if (fcntl(sockfd,F_SETFL,opts) < 0)
        return -1;

    return 0;
}
