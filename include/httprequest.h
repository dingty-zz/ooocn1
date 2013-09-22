#ifndef _HTTPREQUEST_H
#define _HTTPREQUEST_H

#include <linkedlist.h>

enum HTTPMETHOD {
    GET,
    HEAD,
    POST
};

enum HTTPSTATE {
    REQ_LINE,
    REQ_HEADER,
    REQ_CONTENT,
    REQ_DONE
};

typedef struct _KVPair {
    char *store;
    char *key;
    char *value;
} KVPair;

typedef struct _HttpRequest {
    char *store;

    char *uri;
    char *method;
    char *httpversion;

    enum HTTPMETHOD httpmethod;
    enum HTTPSTATE state;

    Linlist *headers;

    char *content;
    int ctLength;
    int ctIndex;
} HttpRequest;

int init_request(HttpRequest *);
void delete_request(HttpRequest *);

KVPair *makeKVPair(char *, int);
void delete_KVP(void *item);
int KVPcompareKey(KVPair *kvp, char *key);
char *KVPgetValue(KVPair *kvp);

char *getValueByKey(Linlist *headers, char *key);

#endif // for #ifndef _HTTPREQUEST_H
