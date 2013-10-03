#include <stdio.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <logger.h>

int ssl_init(SSL_CTX **ssl_context,
             char *ssl_port, int *ssl_fd, char *pkey, char *crt) {
    /************ VARIABLE DECLARATIONS ************/
    int sock;
    struct sockaddr_in addr;
    /************ END VARIABLE DECLARATIONS ************/

    /************ SSL INIT ************/
    SSL_load_error_strings();
    SSL_library_init();

    /* we want to use TLSv1 only */
    if ((*ssl_context = SSL_CTX_new(TLSv1_server_method())) == NULL)
    {
        logger(LOG_ERROR, "Error creating SSL context.\n");
        return -1;
    }

    /* register private key */
    if (SSL_CTX_use_PrivateKey_file(*ssl_context, pkey,
                                    SSL_FILETYPE_PEM) == 0)
    {
        SSL_CTX_free(*ssl_context);
        logger(LOG_ERROR, "Error associating private key.\n");
        return -1;
    }

    /* register public key (certificate) */
    if (SSL_CTX_use_certificate_file(*ssl_context, crt,
                                     SSL_FILETYPE_PEM) == 0)
    {
        SSL_CTX_free(*ssl_context);
        logger(LOG_ERROR, "Error associating certificate.\n");
        return -1;
    }
    /************ END SSL INIT ************/

    /************ SERVER SOCKET SETUP ************/
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        SSL_CTX_free(*ssl_context);
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(ssl_port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        SSL_CTX_free(*ssl_context);
        return -1;
    }

    if (listen(sock, 5))
    {
        SSL_CTX_free(*ssl_context);
        return -1;
    }

    *ssl_fd = sock;
    /************ END SERVER SOCKET SETUP ************/
    return 0;
}

int client_context_init(SSL_CTX *ssl_context, SSL **client_context, int client_sock) {
    /************ WRAP SOCKET WITH SSL ************/
    if ((*client_context = SSL_new(ssl_context)) == NULL)
    {
        logger(LOG_ERROR, "Error creating client SSL context.\n");
        return -1;
    }

    if (SSL_set_fd(*client_context, client_sock) == 0)
    {
        SSL_free(*client_context);
        logger(LOG_ERROR, "Error creating client SSL context.\n");
        return -1;
    }

    if (SSL_accept(*client_context) <= 0)
    {
        SSL_free(*client_context);
        logger(LOG_ERROR, "Error accepting (handshake) client SSL context.\n");
        return -1;
    }
    /************ END WRAP SOCKET WITH SSL ************/

    return 0;
}
