#include <httprequest.h>
#include <httpresponse.h>

void process_request(
    HttpRequest *request, HttpResponse *response, char *buf, int *);

void process_response(
    HttpRequest *request, HttpResponse *response, char *buf, int *);

void start_pipelining(HttpRequest *request, HttpResponse *);
