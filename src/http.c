#include <http.h>
#include <logger.h>
#include "http_internal.h"
#include <httprequest.h>
#include <httpresponse.h>



/*
@brief
  This function handles two kinds of process:
  1. Parsing flow in data into http request.
  2. After request is parsed, based on it we prepare the http response.
 */
void http_process(ClientSocket *clisock) {

    process_request( & clisock->request, & clisock->response,
                     clisock->readbuf, & clisock->readIndex);

    process_response( & clisock->request, & clisock->response,
                     clisock->writebuf, &clisock->writeIndex);

}
