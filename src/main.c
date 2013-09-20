/*
 *
@brief
  This is the main function file of the web server program.
  It first start the server given the listening port.
  Then it loops to service different kinds of net connections.

@author Hongchao Deng (Timber) <hongchad@andrew.cmu.edu>

@bugs No known bugs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <logger.h>

#include <netservice.h>

int main(int argc, char* argv[])
{
    int port;

    port = atoi(argv[1]);

    if( start_server(port) < 0 ) {
        logger(LOG_ERROR, "Failed starting server on port (%d)", port);
        exit(0);
    }

    logger(LOG_INFO, "Start listening on port (%d)", port);

    while (1) {
        net_handle();
    }

    exit(0);
}
