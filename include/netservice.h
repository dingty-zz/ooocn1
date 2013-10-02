/*
 *
@brief

@author Hongchao Deng (Timber) <hongchad@andrew.cmu.edu>

@bugs No known bugs
 *
 */
#ifndef _NETSERVICE_H
#define _NETSERVICE_H

int start_server(int port);
void net_handle();

void set_port_str(char *str);
char *get_server_port();
char* get_cgiProgram();

#endif // for #ifndef _NETSERVICE_H
