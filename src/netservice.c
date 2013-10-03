#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <openssl/ssl.h>

#include <netservice.h>
#include <selectpool.h>
#include <logger.h>
#include <clientsocket.h>
#include <http.h>
#include <ssl.h>

#define LISTENQ 10

SelectPool pool;
char *port_str;
char *ssl_port_str;

char* get_server_port() {return port_str;}
char* get_ssl_port() {return ssl_port_str;}

char *cgi_filename;

char* get_cgiProgram(){
    return cgi_filename;
}

char *WWW_FOLDER;

char* get_WWW_folder(){
    return WWW_FOLDER;
}


/*
@brief
  Start running the server:
  - open the listening port
  - initialize environment

@param
  port: the port to listen;
@return
  if success, return 0;
  if on Unix error, return -1;
*/
int start_server(char *port, char *ssl_port, char *www_folder, char *cgi_name,
  char *pkey, char *crt) {
    int listenfd;
    struct sockaddr_in serveraddr;

    port_str = port;
    ssl_port_str = ssl_port;
    cgi_filename = cgi_name;
    WWW_FOLDER = www_folder;


    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Listenfd will be an endpoint for all requests to port
    on any IP address for this host */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) atoi(port) );

    if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;

    if( ssl_init(&pool.ssl_context, ssl_port, & pool.ssl_fd, pkey, crt) < 0 )
        return -1;
    /* initialize the selectpool */
    init_pool(&pool, listenfd);

    signal(SIGPIPE, SIG_IGN);

    return 0;
}

/*
@brief
  This function service reading and writing of client sockets.
 */
static void net_service(SelectPool *pool) {
    ClientSocket *clisock;
    ll_Node *iter;

    FOR_EACH_CLIENT(pool, iter, clisock) {

        if (FD_ISSET(clisock->fd, & pool->read_set) ||
          (clisock->isHTTPS && SSL_pending(clisock->ssl)) ) {
            logger(LOG_DEBUG, "Client %d reading", clisock->fd);
            handleread(clisock);
        }

        if ( ! clisock->closed ){
            http_process(clisock);
        }

        if ( ! clisock->closed
                && FD_ISSET(clisock->fd, & pool->write_set)) {
            logger(LOG_DEBUG, "Client %d writing", clisock->fd);
            handlewrite(clisock);
        }
    }

    removeClosedSocket(pool);
}

void net_handle() {
    refresh_select(&pool);

    if(pool.nready < 0)
        return;

    net_service(&pool);

    accept_newclient(&pool);
}


