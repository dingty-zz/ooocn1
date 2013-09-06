#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <netservice.h>
#include <selectpool.h>

#define LISTENQ 10

SelectPool pool;

/*
@brief
  open and return a listening socket on port;
@param
  port: the port to listen;
@return
  if success, return 0;
  if on Unix error, return -1;
*/
int open_listening_port(int port) {
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


void net_handle() {
    refresh_select(&pool);
    echo_back(&pool);
}
