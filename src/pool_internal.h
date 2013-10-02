#ifndef _POOL_INTERNAL_H
#define _POOL_INTERNAL_H

#include <netinet/in.h>

void prepare_select(SelectPool *pool);
int add_client(SelectPool *pool, int connfd, struct in_addr ip);

#endif // for #ifndef _POOL_INTERNAL_H
