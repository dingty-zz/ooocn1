#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <selectpool.h>

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

    FD_ZERO(&pool->read_set);
    FD_SET(listenfd, &pool->read_set);

    pool->client_num = 0;
}

static int maxfd_inpool(SelectPool *pool) {
    return (pool->client_num == 0)? pool->listenfd:
           MAX( pool->listenfd, pool->clientfd[0] );
}

static void add_heap(int *heap, int *size, int item) {
    int i,j;

    heap[*size] = item;
    i = *size;
    (*size) ++;

    while(i > 0) {
        j = (i-1) / 2;
        if (heap[i] > heap[j]) {
            SWAPINT(heap[i], heap[j]);
            i = j;
        }
        else
            break;
    }
}

static int setnonblocking(int sockfd) {
    int opts;
    opts = fcntl(sockfd, F_GETFL);

    if (opts < 0)
        return -1;

    opts = (opts | O_NONBLOCK);

    if (fcntl(sockfd,F_SETFL,opts) < 0)
        return -1;

    return 0;
}
static void add_client(SelectPool *pool, int connfd) {
    // we can't handle additional clients
    if( pool->client_num == FD_SETSIZE ) {
        return;
    }
    if( setnonblocking(connfd) < 0) {
        return;
    }

    pool->nready --;
    FD_SET(connfd, &pool->read_set);

    // add that descriptor to heap
    add_heap(pool->clientfd, & pool->client_num, connfd);
}


static void remove_client(SelectPool *pool, int fd) {
  FD_CLR(fd, &pool->read_set);
}

/*
@brief
  This function refresh the select pool by:
  1. add newly connected client socket in
  2. mark those client sockets that has input ready to read
@param
  pool: the select pool
*/
void refresh_select(SelectPool *pool) {
    int connfd;
    struct timeval timeout;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    /*Wait for listening/connected descriptor(s) to become ready*/
    pool->ready_set = pool->read_set;

    if( (pool->nready =
                select(maxfd_inpool(pool)+1, &(pool->ready_set), NULL, NULL, &timeout) )
            < 0
      ) {
        exit(0);
    }

    /*If listening descriptor ready, add new client to pool*/
    if (FD_ISSET(pool->listenfd, &pool->ready_set)) {
        if((connfd =
                    accept(pool->listenfd, (struct sockaddr *)&addr, &addr_len))
                < 0
          ) {
            exit(0);
        }

        add_client(pool, connfd);
    }
}

/*
@brief
  echo back the bytes that clients sent
@param
  pool: the select pool
*/
void echo_back(SelectPool *pool) {
    int i;
    int fd;
    int n;
    char buf[BUFSIZE];

    for (i = pool->client_num - 1; i >= 0 && pool->nready > 0; i--) {
        fd = pool->clientfd[i];
        if(FD_ISSET(fd, &pool->ready_set)) {
            pool->nready --;
            printf("readwrite\n");
            n = read(fd, buf, BUFSIZE);
            if(n == 0) {
                remove_client(pool, fd);
            }
            else {
                write(fd, buf, n);
            }
        }
    }
}
