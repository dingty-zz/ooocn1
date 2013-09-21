#ifndef _HTTPRESPONSE_H
#define _HTTPRESPONSE_H

#include <stdio.h>
#include <linkedlist.h>

typedef struct _HttpResponse {
    int httpcode;
    int state;
    int bufIndex; // used to record where last time current string left
    int preprocessed;

    char *datestr;
    char *lmdate;
    FILE *fp;
    int fsize;
    char *ctlen;

    char *mimetype;

} HttpResponse;

void init_response(HttpResponse *);
void delete_response(HttpResponse *);

#endif // for #ifndef _HTTPRESPONSE_H
