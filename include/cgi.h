#ifndef _CGI_H
#define _CGI_H

#include <clientsocket.h>

int is_cgi( HttpRequest *request, HttpResponse *response);
int process_cgi(ClientSocket *);

#endif // for #ifndef _CGI_H
