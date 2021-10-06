#include "node.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* constructor for a node */
Node *node_create(char *oldspeak, char *newspeak) {
    Node *n = (Node *) malloc(sizeof(Node));

    if (n) {

        /* default values */
        n->next = NULL;
        n->prev = NULL;
        n->oldspeak = NULL;
        n->newspeak = NULL;

        /* copy oldspeak into node if possible using strndup */
        if (oldspeak) {
            size_t old_len = strlen(oldspeak); // size of the string
            n->oldspeak = strndup(oldspeak, old_len); // copy it in oldspeak
        }

        /* do the same as above with newspeak */
        if (newspeak) {
            size_t new_len = strlen(newspeak);
            n->newspeak = strndup(newspeak, new_len);
        }

        /* could not allocate memory */
        if ((oldspeak && !n->oldspeak) || (newspeak && !n->newspeak)) {
            free(n);
            n = NULL;
        }
    }

    return n;
}

/* destructor for a node */
void node_delete(Node **n) {
    if (n && *n) {
        /* free the strings */
        if ((*n)->oldspeak)
            free((*n)->oldspeak);
        if ((*n)->newspeak)
            free((*n)->newspeak);
        (*n)->oldspeak = NULL;
        (*n)->newspeak = NULL;

        /* free the node */
        free(*n);
        *n = NULL;
    }
    return;
}

/* prints a node */
void node_print(Node *n) {
    if (!n)
        return;

    /* for printing rightspeak */
    if (n->oldspeak && n->newspeak) {
        fprintf(stdout, "%s->%s\n", n->oldspeak, n->newspeak);
    }

    /* for printing badspeak */
    else {
        fprintf(stdout, "%s\n", n->oldspeak);
    }

    return;
}
