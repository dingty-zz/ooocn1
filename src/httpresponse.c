#include <httpresponse.h>

void init_response(HttpResponse *response) {
    response->state = 0;
    response->bufIndex = 0;
}
