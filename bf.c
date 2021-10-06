#include "bf.h"

#include "bv.h"
#include "speck.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUM_SALTS 3 // total num of salts

/* tracks number of bits set in the BV (to be used by bf_count) */
static uint32_t bit_count = 0;

/* credits: provided in the lab documentation */
/* BloomFilter (BF) definition */
struct BloomFilter {
    uint64_t primary[2]; // salts for the hash function
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter; // the underlying BitVector (BV)
};

/* credits: provided in the lab documentation */
/* constructor for the BF */
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));

    if (bf) {
        /* salts from the lab doc */
        bf->primary[0] = 0x5adf08ae86d36f21;
        bf->primary[1] = 0xa267bbd3116f3957;

        bf->secondary[0] = 0x419d292ea2ffd49e;
        bf->secondary[1] = 0x09601433057d5786;

        bf->tertiary[0] = 0x50d8bb08de3818df;
        bf->tertiary[1] = 0x4deaae187c16ae1d;

        /* make the bit vector */
        bf->filter = bv_create(size);
        if (!bf->filter) {
            free(bf);
            bf = NULL;
        }
    }

    return bf;
}

/* destructor for the BF */
void bf_delete(BloomFilter **bf) {
    if (bf && *bf && (*bf)->filter) {
        bit_count = 0; // reset the bit count (for bf_count)
        bv_delete(&((*bf)->filter)); // delete the BV
        free(*bf);
        *bf = NULL;
    }
    return;
}

/* returns the size of the BF */
uint32_t bf_size(BloomFilter *bf) {
    if (!bf)
        return 0; // no BF
    return bv_length(bf->filter); // size == length of the underlying BV
}

/* adds the word oldspeak to the BF */
void bf_insert(BloomFilter *bf, char *oldspeak) {
    if (!bf || !oldspeak)
        return; // safety check

    uint32_t index, size = bf_size(bf);
    uint64_t *salt[NUM_SALTS]
        = { bf->primary, bf->secondary, bf->tertiary }; // temporary salt pointer storing array

    // for each salt
    for (uint8_t i = 0; i < NUM_SALTS; i++) {

        /* get the index by hashing with ith salt */
        index = hash(salt[i], oldspeak) % size;

        if (!bv_get_bit(bf->filter, index)) { // if the bit is not set already
            bit_count++; // bit wasnt set. now newly added (for bf_count)
            bv_set_bit(bf->filter, index); // set the bit @ index in the bv
        }
    }

    return;
}

/* checks if the word oldspeak has been added to the BF */
bool bf_probe(BloomFilter *bf, char *oldspeak) {
    if (!bf || !oldspeak)
        return false; // safety check

    uint32_t index, size = bf_size(bf);
    uint64_t *salt[NUM_SALTS]
        = { bf->primary, bf->secondary, bf->tertiary }; // temporary salt pointer storing array

    // with each salt
    for (uint8_t i = 0; i < NUM_SALTS; i++) {
        /* get the index by hashing with ith salt */
        index = hash(salt[i], oldspeak) % size;
        if (!bv_get_bit(bf->filter, index))
            return false; // bit not set therefore not added
    }

    return true; // all bits set
}

/* returns number of bits set in the BF */
uint32_t bf_count(BloomFilter *bf) {
    if (!bf)
        return 0; // safety check
    return bit_count; // the variable that has been tracking the bit count
}

/* prints the BF */
void bf_print(BloomFilter *bf) {
    if (bf)
        bv_print(bf->filter); // print the bv
    return;
}
