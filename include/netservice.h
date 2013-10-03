/*
 *
@brief

@author Hongchao Deng (Timber) <hongchad@andrew.cmu.edu>

@bugs No known bugs
 *
 */
#ifndef _NETSERVICE_H
#define _NETSERVICE_H

int start_server(char *, char *, char*, char*, char*, char*);
void net_handle();

char* get_server_port();
char* get_ssl_port();
char* get_cgiProgram();
char* get_WWW_folder();

#endif // for #ifndef _NETSERVICE_H
