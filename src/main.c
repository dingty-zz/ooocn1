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
    if(argc < 3) return 0;

    if( log_init(argc < 4? "": argv[3]) < 0 ){
    }


    if( start_server(argv[1], argv[2], "ssl/hongchad.key", "ssl/hongchad.crt") < 0 ) {
        exit(0);
    }

    logger(LOG_INFO, "Server started listening...");

    while (1) {
        net_handle();
    }

    exit(0);
}
