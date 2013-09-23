#include <stdlib.h>
#include <httpresponse.h>

void init_response(HttpResponse *response) {
    response->state = 0;
    response->bufIndex = 0;
    response->preprocessed = 0;

    response->datestr = NULL;
    response->lmdate = NULL;
    response->fp = NULL;
    response->ctlen = NULL;
    response->isPipelining = 0;
}

void delete_response(HttpResponse *response) {
    if(response->datestr) free(response->datestr);
    if(response->lmdate) free(response->lmdate);
    if(response->ctlen) free(response->ctlen);
    if(response->fp) fclose(response->fp);
}
