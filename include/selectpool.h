#ifndef _SELECTPOOL_H
#define _SELECTPOOL_H

#include <sys/select.h>
#include <linkedlist.h>
#include <clientsocket.h>

typedef struct _SelectPool { /*represents a pool of connected descriptors*/

    int listenfd;

    fd_set read_set;
    fd_set write_set;
    int nready;

    Linlist *clients;

} SelectPool;

void init_pool(SelectPool *, int);
void refresh_select(SelectPool *);
void accept_newclient(SelectPool *);

#define FOR_EACH_CLIENT(pool, iter, client) \
    iter = ll_start( pool->clients ); \
    for ( client = (ClientSocket *) iter->item; \
          iter != ll_end(pool->clients) ; \
          iter = ll_next(iter), client = (ClientSocket *) iter->item \
        )

void removeClosedSocket(SelectPool *pool);

#endif // for #ifndef _SELECTPOOL_H
