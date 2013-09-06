#ifndef _SELECTPOOL_H
#define _SELECTPOOL_H

#include <sys/select.h>

typedef struct _SelectPool { /*represents a pool of connected descriptors*/

    int listenfd;

    fd_set read_set; /*set of all active descriptors*/
    fd_set ready_set; /*subset of descriptors ready for reading*/
    int nready; /*number of ready descriptors from select*/

    int client_num; /*number of clients*/
    int clientfd[FD_SETSIZE]; /*set of active descriptors*/

} SelectPool;

void init_pool(SelectPool *, int);
void refresh_select(SelectPool *);
void echo_back(SelectPool *);

#endif // for #ifndef _SELECTPOOL_H
