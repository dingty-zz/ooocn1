#include <stdlib.h>
#include <malloc.h>
#include <httprequest.h>
#include <string.h>

void init_request(HttpRequest *request) {
    init_linkedlist(&request->headers);
    request->state = REQ_LINE;
    request->store = NULL;
    request->content = NULL;
    request->cgi = 0;
}

void delete_KVP(void *item) {
    KVPair *kvp = (KVPair *) item;
    free(kvp->store);
    free(kvp);
}

void delete_request(HttpRequest *request) {
    if(request->store ) free(request->store);
    if(request->content ) free(request->content);
    ll_delete_allnodes(&request->headers, delete_KVP);
}

int KVPcompareKey(KVPair *kvp, char *key) {
    return strcasecmp(kvp->key, key);
}

KVPair *makeKVPair(char *buf, int len) {
    KVPair * kvp;

    kvp = malloc(sizeof(KVPair));
    if(! kvp) return NULL;

    kvp->store = malloc(len - 1); // no "\r\n"
    if(! kvp->store) {
        free(kvp);
        return NULL;
    }

    // "key: value"
    memcpy(kvp->store, buf, len - 2);
    kvp->store[len-2] = '\0';

    kvp->key = kvp->store;
    kvp->value = strchr(kvp->store, ':');
    if(kvp->value == NULL){
        free(kvp);
        return NULL;
    }
    *(kvp->value) = '\0';
    kvp->value += 1 + strspn(kvp->value + 1, " ");

    return kvp;
}

char *KVPgetValue(KVPair *kvp){
    return kvp->value;
}

char *getValueByKey(Linlist *headers, char *key){
    ll_Node *iter;
    iter = ll_start(headers);
    while( iter != ll_end(headers)){
        if( KVPcompareKey(iter->item, key) ){
            return KVPgetValue(iter->item);
        }
        iter = ll_next(iter);
    }
    return NULL;
}
