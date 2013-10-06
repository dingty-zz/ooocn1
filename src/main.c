/*
 *
@brief
  This is the entry point file of the web server program.

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

    // we need to daemonize it first before we initialize the logger.
    if( daemonize(argv[4]) != EXIT_SUCCESS){
        printf("Can't daemonize");
        exit(EXIT_FAILURE);
    }

    if( log_init(argv[3]) < 0 ){
        printf("Can't initialize logger\n");
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
