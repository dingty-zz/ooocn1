#ifndef _POOL_INTERNAL_H
#define _POOL_INTERNAL_H

void prepare_select(SelectPool *pool);
int add_client(SelectPool *pool, int connfd);

#endif // for #ifndef _POOL_INTERNAL_H
