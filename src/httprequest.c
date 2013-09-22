#include <stdlib.h>
#include <malloc.h>
#include <httprequest.h>
#include <string.h>

int init_request(HttpRequest *request) {
    request->headers = new_Linlist();
    if( ! request->headers ) {
        return -1;
    }
    request->state = REQ_LINE;
    request->store = NULL;
    request->content = NULL;
    return 0;
}

void delete_KVP(void *item) {
    KVPair *kvp = (KVPair *) item;
    free(kvp->store);
    free(kvp);
}

void delete_request(HttpRequest *request) {
    if(request->store ) free(request->store);
    if(request->content ) free(request->content);
    ll_delete_allnodes(request->headers, delete_KVP);
    free(request->headers);
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