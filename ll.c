#include "ll.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) (a > b ? a : b) // returns max

/* extern var for stats */
uint64_t seeks = 0; // number of seeks performed
uint64_t links = 0; // number of links traversed

/* credits: provided in the lab documentation */
/* LinkedList (LL) definition */
struct LinkedList {
    uint32_t length; // length of LL
    Node *head; // sentinel nodes
    Node *tail;
    bool mtf; // move to front or not
};

/* constructor for the LL */
LinkedList *ll_create(bool mtf) {
    LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));

    if (ll) {
        ll->length = 0;
        ll->mtf = mtf;

        /* create sentinel nodes */
        ll->head = node_create(NULL, NULL);
        ll->tail = node_create(NULL, NULL);

        /* could not allocate memory */
        if (!ll->head || !ll->tail) {
            free(ll);
            ll = NULL;
        }

        /* else connect the sentinel nodes and make NULL<-head<=>tail->NULL */
        else {
            ll->head->prev = NULL;
            ll->head->next = ll->tail;
            ll->tail->next = NULL;
            ll->tail->prev = ll->head;
        }
    }

    return ll;
}

/* destructor for the LL */
void ll_delete(LinkedList **ll) {

    if (ll && *ll) {

        Node *curr = (*ll)->head->next; // tracks the curr node

        /* frees each but the tail node */
        while (curr != NULL) {
            node_delete(&(curr->prev)); // free the previous node
            curr = curr->next; // move on
        }

        node_delete(&((*ll)->tail)); // delete the tail

        free(*ll);
        *ll = NULL; // done with ll
    }

    return;
}

/* returns the length of the LL */
uint32_t ll_length(LinkedList *ll) {
    if (!ll)
        return 0; // no ll
    return ll->length;
}

/* helper function to move a node to front */
static void move_to_front(LinkedList *ll, Node *n) {
    if (!ll || !n)
        return;

    /* unattach the node */
    n->prev->next = n->next;
    n->next->prev = n->prev;

    /* attach the node at front */
    n->next = ll->head->next;
    n->prev = ll->head;

    /* change the links of surrounding nodes */
    ll->head->next = n;
    n->next->prev = n;

    return;
}

/* looks up if node with given paramenters is in the LL */
Node *ll_lookup(LinkedList *ll, char *oldspeak) {
    seeks++; // update number of lookups performed

    if (!ll || !oldspeak)
        return NULL; // safety check

    Node *temp = ll->head->next;
    size_t old_size = strlen(oldspeak);
    bool found = false;

    /* linearly search for oldspeak string */
    while (temp != ll->tail) {

        /* found a match */
        if (temp->oldspeak
            && !strncmp(oldspeak, temp->oldspeak, MAX(old_size, strlen(temp->oldspeak)))) {
            found = true;
            break;
        }

        temp = temp->next;
        links++; // increment links traversed
    }

    /* move to front if mtf == true */
    if (ll->mtf && found)
        move_to_front(ll, temp);

    return found ? temp : NULL; // if node found, return it else null
}

/* adds a node with the given parameters after head sentinel node of the LL */
void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
    if (!ll || ll_lookup(ll, oldspeak))
        return; // no LL or node already in LL

    Node *n = node_create(oldspeak, newspeak); // else create a new node

    /* attach the node at the front */
    n->next = ll->head->next;
    n->prev = ll->head;

    /* readjust the surrounding node links */
    ll->head->next = n;
    n->next->prev = n;

    ll->length++;

    return;
}

/* prints the LL */
void ll_print(LinkedList *ll) {
    if (!ll)
        return;

    Node *temp = ll->head->next;

    /* prints each node except the head and tail */
    while (temp != ll->tail) {
        node_print(temp);
        temp = temp->next; // move to next node
    }

    return;
}
