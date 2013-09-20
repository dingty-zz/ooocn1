#include <httprequest.h>
#include <httpresponse.h>
#include <clientsocket.h>

void process_response(HttpRequest *request, HttpResponse * response,
                      char *buf, int *lenptr) {
    if(request->state != REQ_DONE) return;
}
