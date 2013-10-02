#include <http.h>
#include <logger.h>
#include "http_internal.h"
#include <httprequest.h>
#include <httpresponse.h>
#include <cgi.h>


/*
@brief
  This function handles two kinds of process:
  1. Parsing flow in data into http request.
  2. After request is parsed, based on it we prepare the http response.
 */
void http_process(ClientSocket *clisock) {

    if(clisock->response.state == -1){
        return;
    }

    process_request( & clisock->request, & clisock->response,
                     clisock->readbuf, & clisock->readIndex);

    if( is_cgi( &clisock->request, &clisock->response) ){
        if ( process_cgi(clisock) == 0 )
            return;
    }

    process_response( & clisock->request, & clisock->response,
                     clisock->writebuf, &clisock->writeIndex);

}
