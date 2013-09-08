#include <malloc.h>

#include <linkedlist.h>

ll_Node* new_ll_Node(void *item) {
    ll_Node *node = malloc(sizeof(ll_Node));
    if(node == NULL)
        return NULL;

    node->item = item;
    return node;
}

Linlist* new_Linlist() {
    Linlist *ll = malloc(sizeof(Linlist));
    if (ll == NULL)
        return NULL;

    ll_Node *head = &ll->head;
    head->next = head;
    head->prev = head;
    ll->count = 0;

    return ll;
}

int ll_count(Linlist *ll) {
    return ll->count;
}

void ll_insert_last(Linlist *ll, ll_Node *node) {
    ll_Node *head, *last;

    head = &ll->head;
    last = head->prev;

    last->next = node;
    node->prev = last;
    node->next = head;
    head->prev = node;

    ll->count ++;
}

void ll_remove(Linlist *ll, ll_Node *node) {
    ll_Node *next, *prev;
    prev = node->prev;
    next = node->next;

    next->prev = prev;
    prev->next = next;

    ll->count --;
}

ll_Node *ll_start(Linlist *ll) {
    ll_Node* head = &ll->head;
    return head->next;
}
ll_Node *ll_next(ll_Node *node) {
    return node->next;
}
ll_Node *ll_end(Linlist *ll) {
    return &ll->head;
}