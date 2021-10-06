#include "ht.h"

#include "ll.h"
#include "speck.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* tracks the number of non-null linked lists in HT (used by ht_count) */
uint32_t total_lls = 0;

/* credits: provided in the lab documentation */
/* HashTable (HT) definition */
struct HashTable {
    uint64_t salt[2]; // salt for the hash function
    uint32_t size; // size of the table
    bool mtf; // move to front ll or not
    LinkedList **lists; // array of linkedlist (LL) pointers
};

/* credits: provided in the lab documentation */
/* constructor for the HT */
HashTable *ht_create(uint32_t size, bool mtf) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));

    if (ht) {
        /* salt from the lab doc */
        ht->salt[0] = 0x9846e4f157fe8840;
        ht->salt[1] = 0xc5f318d7e055afb8;

        ht->size = size;
        ht->mtf = mtf;

        ht->lists = (LinkedList **) calloc(size, sizeof(LinkedList *)); // array of ll pointers

        /* cannot allocate memory */
        if (!ht->lists) {
            free(ht);
            ht = NULL;
        }
    }

    return ht;
}

/* destructor for the HT */
void ht_delete(HashTable **ht) {

    if (ht && *ht && (*ht)->lists) {
        for (uint32_t i = 0; i < (*ht)->size; i++) {
            ll_delete(&((*ht)->lists[i])); // delete each linked list
        }

        /* free the array and the ht */
        free((*ht)->lists);
        free((*ht));
        *ht = NULL;

        total_lls = 0; // reset the count
    }

    return;
}

/* returns the size of the HT */
uint32_t ht_size(HashTable *ht) {
    if (!ht)
        return 0; // no ht
    return ht->size;
}

/* looks up if node is in the LL in the HT */
Node *ht_lookup(HashTable *ht, char *oldspeak) {
    if (!ht || !oldspeak)
        return NULL; // safety check

    uint32_t index = hash(ht->salt, oldspeak) % ht->size; // get the linked list index by hashing

    /* if no LL at that index return NULL, else lookup in LL and return the result */
    if (!(ht->lists[index]))
        return NULL;
    else
        return ll_lookup(ht->lists[index], oldspeak);
}

/* adds a node with the given parameters into a HT LinkedList */
void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
    if (!ht || !oldspeak)
        return; // safety check

    uint32_t index = hash(ht->salt, oldspeak) % ht->size; // get the linked list index by hashing
    LinkedList *ll = ht->lists[index]; // list at the index

    /* no linked list, make one and add it to the HT */
    if (!ll) {
        ll = ll_create(ht->mtf);
        ht->lists[index] = ll;
        total_lls++; // was null now it is not. therefore increment
    }

    ll_insert(ll, oldspeak, newspeak); // insert in the list

    return;
}

/* returns number of LLs added in the HT */
uint32_t ht_count(HashTable *ht) {
    if (!ht)
        return 0; // no ht
    return total_lls; // variable that has been tracking non-null LLs
}

/* prints the HT (only non null LLs) */
void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht->size; i++) {
        fprintf(stdout, "\n[%u]\n", i); // to make it more clear
        ll_print(ht->lists[i]); // print each ll (will only print if ll not null)
    }
    return;
}
