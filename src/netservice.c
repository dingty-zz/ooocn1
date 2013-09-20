#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <netservice.h>
#include <selectpool.h>
#include <logger.h>
#include <clientsocket.h>
#include <http.h>

#define LISTENQ 10

SelectPool pool;

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
int start_server(int port) {
    int listenfd;
    struct sockaddr_in serveraddr;

    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Listenfd will be an endpoint for all requests to port
    on any IP address for this host */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;

    /* initialize the selectpool */
    init_pool(&pool, listenfd);

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

        if (FD_ISSET(clisock->fd, & pool->read_set)) {
            logger(LOG_DEBUG, "Client %d reading", clisock->fd);
            handleread(clisock);
        }

        if ( ! isClosed(clisock) ){
            http_process(clisock);
        }

        if ( ! isClosed(clisock)
                && FD_ISSET(clisock->fd, & pool->write_set)) {
            logger(LOG_DEBUG, "Client %d writing", clisock->fd);
            handlewrite(clisock);
        }
    }

    removeClosedSocket(pool);
}

void net_handle() {
    refresh_select(&pool);

    if(pool.nready <= 0)
        return;

    net_service(&pool);

    accept_newclient(&pool);
}
