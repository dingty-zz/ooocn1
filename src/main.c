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
#include <daemonize.h>

#include <netservice.h>

int main(int argc, char* argv[])
{
    if(argc < 9)
        exit(EXIT_FAILURE);


    if( log_init(argv[3]) < 0 ){
        printf("Can't initialize logger\n");
        exit(EXIT_FAILURE);
    }

    if( daemonize(argv[4]) != EXIT_SUCCESS){
        logger(LOG_ERROR, "Can't daemonize");
        exit(EXIT_FAILURE);
    }


    if( start_server(argv[1], argv[2], argv[5], argv[6], argv[7], argv[8]) < 0 ) {
        logger(LOG_ERROR, "Can't start server");
        exit(0);
    }

    logger(LOG_INFO, "Server started listening...");

    while (1) {
        net_handle();
    }

    exit(0);
}
