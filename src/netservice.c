#include <netservice.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define LISTENQ 10

/*
@brief
  open and return a listening socket on port
@param
  port: the port to listen
@return
  if success, return socket file descriptor (non-negative) of listing port
  if on Unix error, return -1 and sets errno
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

    return listenfd;
}
