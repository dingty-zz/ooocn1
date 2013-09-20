#ifndef _HTTPRESPONSE_H
#define _HTTPRESPONSE_H

#include <linkedlist.h>

typedef struct _HttpResponse {
    int httpcode;
    int state;
    int bufIndex;
} HttpResponse;

void init_response(HttpResponse *);

#endif // for #ifndef _HTTPRESPONSE_H
