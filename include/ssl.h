#ifndef _SSL_H
#define _SSL_H

#include <openssl/ssl.h>

int ssl_init(SSL_CTX **, char *, int*, char*, char*);
int client_context_init(SSL_CTX *, SSL **, int );

#endif // for #ifndef _SSL_H
