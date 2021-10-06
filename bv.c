#include "bv.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE 8

/* CREDITS: based on the definition provided in the lab doc */
struct BitVector {
    uint32_t length; // length in bits
    uint8_t *vector;
};

/* constructor for a BitVector */
BitVector *bv_create(uint32_t length) {
    BitVector *v = (BitVector *) malloc(sizeof(BitVector));

    if (v) {
        v->length = length;

        uint32_t min_elems
            = length / BYTE + 1; // to get size for array (credits: based on lab5 doc)
        v->vector = (uint8_t *) calloc(min_elems, sizeof(uint8_t));

        /* initialize each to zero (use memset instead? rem comment) */
        for (uint32_t i = 0; i < min_elems; i++) {
            v->vector[i] = 0;
        }

        if (!v->vector) {
            free(v);
            v = NULL;
        }
    }
    return v;
}

/* destructor for a BitVector */
void bv_delete(BitVector **v) {
    if (*v && (*v)->vector) {
        free((*v)->vector);
        free(*v);
        *v = NULL;
    }
    return;
}

/* return BV length (in bits) */
uint32_t bv_length(BitVector *v) {
    if (!v)
        return 0; // no vector
    return v->length;
}

/* sets the bit at index i */
void bv_set_bit(BitVector *v, uint32_t i) {
    if (i < bv_length(v)) // cant set if i > length vector
        v->vector[i / BYTE]
            |= ((uint8_t) 1 << (i % BYTE)); // based on hints from set.c given in asgn3 and lab5
    return;
}

/* clears the bit at index i */
void bv_clr_bit(BitVector *v, uint32_t i) {
    if (i < bv_length(v))
        v->vector[i / BYTE]
            &= ~((uint8_t) 1 << (i % BYTE)); // based on hints from set.c given in asgn3 and lab5
    return;
}

/* gets the bit at index i */
uint8_t bv_get_bit(BitVector *v, uint32_t i) {
    return (v->vector[i / BYTE] & ((uint8_t) 1 << (i % BYTE)))
               ? 1
               : 0; // based on hints from set.c given in asgn3 and lab5
}

/* prints the BitVector (each bit) */
void bv_print(BitVector *v) {
    uint32_t i = bv_length(v);
    while (i >= 1) {
        i--;
        fprintf(stdout, "%c", bv_get_bit(v, i) ? '1' : '0');
    }
    fprintf(stdout, "\n");
}
