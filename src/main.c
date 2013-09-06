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

#include <netservice.h>


int main(int argc, char* argv[])
{
    int port;

    port = atoi(argv[1]);

    if( open_listening_port(port) < 0 ) {
      exit(0);
    }

    while (1) {
        net_handle();
    }

    exit(0);
}
