#include <httprequest.h>
#include <httpresponse.h>
#include <clientsocket.h>

static int addBuffer(char *, int *, int *, char *);


void process_response(HttpRequest *request, HttpResponse * response,
                      char *buf, int *lenptr) {
    if(request->state != REQ_DONE) return;

    // status line
    if(response->state == 0) {
        if (! addBuffer(buf, lenptr, & response->bufIndex,
                        "HTTP/1.1 200 OK\r\n"))
            return;

        response->state = (response->httpcode != 200)? 1: 2 ;
    }
    if(response->state == 1){
        if (! addBuffer(buf, lenptr, & response->bufIndex,
                        "\r\n"))
            return;
        response->state = -1; // response done
        return;
    }

    // response state == 2
    switch (request->httpmethod) {
    case HEAD:

        break;
    case GET:

        break;
    case POST:

        break;
    }
}
