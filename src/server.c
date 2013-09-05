/*
 *
@brief

@author Hongchao Deng (Timber) <hongchad@andrew.cmu.edu>

@bugs No known bugs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>


#include <netservice.h>


int main(int argc, char* argv[])
{
    int port;

    port = atoi(argv[1]);

    open_listening_port(port);

    while (1) {
        printf("accepting clients\n");
        // TODO: select
        // TODO: check if listenfd is set
        // TODO: check clients
    }

    return EXIT_SUCCESS;
}
