#ifndef _POOL_INTERNAL_H
#define _POOL_INTERNAL_H

int maxfd_inpool(SelectPool *pool);
void set_readwrite(SelectPool *pool);
int add_client(SelectPool *pool, int connfd);
int setnonblocking(int sockfd);

#endif // for #ifndef _POOL_INTERNAL_H
