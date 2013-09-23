#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <httprequest.h>
#include <httpresponse.h>
#include <clientsocket.h>
#include <logger.h>

static int checkBuffer(char *, int);
static void process_reqline(HttpRequest *request, int len);
static int verify_method(HttpRequest *);
static int verify_uri(char *);
static int verify_httpV(char *);
static int isValidCtLen(char *);
static int read_contentData(char *, int *, char *, int *, int);

static int checkBuffer(char *buf, int len) {
    if(buf[len-1] == '\n') {
        if(len - 2 < 0 || buf[len-2] != '\r') {
            return 0; // bad request
        }
        return 1; // okay
    } else {
        if(len == CLISOCK_BUFSIZE) {
            return 0; // bad request
        }
        return 2; // need more data
    }
}

/*
@brief
  This function provides the state machine of handling request data.
*/
void process_request( HttpRequest *request, HttpResponse *response,
                      char *buf, int *lenptr) {
    int ret;
    int len = *lenptr;
    char *ctLengthStr;

    if(request->state == REQ_DONE) return;
    if(len == 0) return;

    switch (request->state) {
    case REQ_LINE:
        ret = checkBuffer(buf, len);
        if( !ret) {
            response->httpcode = 400;
            request->state = REQ_DONE;
            return;
        }
        if(ret != 1) return; //not ready

        request->store = malloc(len-1); // no "\r\n"
        if( ! request->store ) {
            response->httpcode = 500;
            request->state = REQ_DONE;
            return;
        }
        memcpy(request->store, buf, len - 2 ); // no "\r\n"
        request->store[len-2] = '\0';

        process_reqline(request, len);

        if( ! verify_method(request)
                || ! verify_uri(request->uri)
                || ! verify_httpV(request->httpversion)) {
            response->httpcode = 400;
            request->state = REQ_DONE;
            return;
        }

        *lenptr = 0;
        request->state = REQ_HEADER;

        logger(LOG_INFO, "NEW REQUEST LINE: %s %s %s",
               request->method, request->uri, request->httpversion);

        break;

    case REQ_HEADER:
        ret = checkBuffer(buf, len);
        if( !ret) {
            response->httpcode = 400;
            request->state = REQ_DONE;
            return;
        }
        if(ret != 1) return; //not ready

        if(len == 2 && buf[0] == '\r' && buf[1] == '\n') {
            logger(LOG_DEBUG, "Header parsed done!");

            *lenptr = 0;
            switch (request->httpmethod) {
            case GET:
            case HEAD:
                response->httpcode = 200;
                request->state = REQ_DONE;
                break;
            case POST:
                request->state = REQ_CONTENT;
                break;

            }
            return;
        }

        KVPair *kvp = makeKVPair(buf, len);
        if(! kvp) {
            response->httpcode = 500;
            request->state = REQ_DONE;
            return;
        }

        ll_Node * node = new_ll_Node(kvp);
        if(! node) {
            delete_KVP(kvp);
            response->httpcode = 500;
            request->state = REQ_DONE;
            return;
        }

        ll_insert_last(&request->headers, node);
        *lenptr = 0;

        logger(LOG_INFO, "REQUEST HEADER (%s: %s)", kvp->key, kvp->value);

        break;

    case REQ_CONTENT:
        if(! request->content) {
            // find content-length.
            ctLengthStr = getValueByKey(&request->headers, "Content-Length");
            // malloc length
            if( ! isValidCtLen(ctLengthStr) ) {
                response->httpcode = 400;
                request->state = REQ_DONE;
                return;
            }
            request->ctLength = atoi(ctLengthStr);
            request->content = malloc(request->ctLength);
            if( request->ctLength < 0 || ! request->content ) {
                response->httpcode = 500;
                request->state = REQ_DONE;
                return;
            }

            request->ctIndex = 0;
        }
        // move data;
        if( !read_contentData(buf, lenptr,
                              request->content, &request->ctIndex, request->ctLength) ) {
            return; // unfinished
        }
        response->httpcode = 200;
        request->state = REQ_DONE;

        break;
    default: // stupid compiler
        return;
    }
}

static int verify_method(HttpRequest *request) {
    if(request->method == NULL) return 0;

    if(strcasecmp(request->method, "GET") == 0 ) {
        request->httpmethod = GET;
        return 1;
    }
    if(strcasecmp(request->method, "HEAD") == 0 ) {
        request->httpmethod = HEAD;
        return 1;
    }
    if(strcasecmp(request->method, "POST") == 0 ) {
        request->httpmethod = POST;
        return 1;
    }

    return 0;
}
static int verify_uri(char *uri) {
    return uri != NULL ;
}
static int verify_httpV(char *version) {
    return version != NULL && strcasecmp(version, "HTTP/1.1") == 0;
}


static void process_reqline(HttpRequest *request, int len) {
    // "method uri HTTP/1.1"
    request->method = request->store;

    request->uri = strchr(request->store, ' ');
    if(! request->uri) return;
    *(request->uri) = '\0';
    request->uri += 1 + strspn(request->uri + 1, " ") ;

    request->httpversion = strchr(request->uri, ' ');
    if(! request->httpversion) return;
    *(request->httpversion) = '\0';
    request->httpversion += 1 + strspn(request->httpversion + 1, " ") ;
}

static int isValidCtLen(char *str) {
    int i;
    int len;
    if(! str) return 0;
    len = strlen(str);
    for (i = 0; i < len; i++) {
        if(str[i] <'0' || str[i] >'9')
            return 0;
    }
    return 1;
}
static int read_contentData(char *buf, int *buflen,
                            char *content, int *ctIndex, int ctLen) {
    int hasToCopy = (*buflen);
    int leftToCopy = ctLen - (*ctIndex);

    if( hasToCopy >= leftToCopy){
        memcpy(content + (*ctIndex), buf, leftToCopy);
        *buflen -= leftToCopy;
        *ctIndex += leftToCopy;
        return 1;
    }
    else{
        memcpy(content + (*ctIndex), buf, hasToCopy);
        *buflen -= hasToCopy;
        *ctIndex += hasToCopy;
        return 0;
    }
}
