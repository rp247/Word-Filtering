#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "parser.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* max size of a word from badspeak and newspeak file words (see design) */
#define MAX_WORD 4096 // max word size is 4096 (newspeak and badspeak words are capped at 28)

/* 
 * valid regex pattern (to be compiled later):
 * [a-zA-Z0-9_]		:	word can start from this set
 * ([-']?[a-zA-Z0-9_])*	:	character group that must follow the valid beginning set
 * [-']?		:	the set can be followed by either at most one continuous - or '
 * [a-zA-Z0-9_]		:	which indeed must be followed by the valid set again (to avoid -- or '')
 * (group)*		:	do this zero or more times (to include single letters and multiple-hypens-too)
 */
#define PATTERN "[a-zA-Z0-9_]+([-']?[a-zA-Z0-9_])*"

/* helper function to print usage */
static void usage(char *argv) {
    fprintf(stdout,
        "SYNOPSIS\n"
        "  A word filtering program for the GPRSC.\n"
        "  Filters out and reports bad words parsed from stdin.\n"
        "\n"
        "USAGE\n"
        "  %s [-hsm] [-t size] [-f size]\n"
        "\n"
        "OPTIONS\n"
        "  -h           Program usage and help.\n"
        "  -s           Print program statistics.\n"
        "  -m           Enable move-to-front rule.\n"
        "  -t size      Specify hash table size (default: 10000).\n"
        "  -f size      Specify Bloom filter size (default: 2^20).\n",
        argv);
}

/* helper functions that frees mem if error occurs in main */
static void main_err(BitVector *args, HashTable *ht, BloomFilter *bf) {
    if (args)
        bv_delete(&args);
    if (ht)
        ht_delete(&ht);
    if (bf)
        bf_delete(&bf);
}

/* helper function to either readin the badspeak file or oldspeak newspeak pair */
static void read_file(FILE *infile, HashTable *ht, BloomFilter *bf, bool is_badfile) {
    char old_speak[MAX_WORD];
    char new_speak[MAX_WORD];

    /* read in each word till eof */
    while (fscanf(infile, "%s", old_speak) != EOF) {
        bf_insert(bf, old_speak); // add to BF

        /* file is badspeak, only add oldspeak to ht */
        if (is_badfile)
            ht_insert(ht, old_speak, NULL); // add to HT (no newspeak yet)

        /* file is oldspeak, get newspeak and add the pair to ht */
        else {
            fscanf(infile, "%s", new_speak);
            ht_insert(ht, old_speak, new_speak);
        }
    }

    fclose(infile); // done with file

    return;
}

/* helper function to lower charecter [A-Z] */
static inline char lower_char(uint8_t c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c; // upper-lower diff = 32
}

/* helper function to make a string lowercase (wrapper around tolower) */
static void lower_str(char *str) {
    uint64_t i = 0;
    char c;
    while ((c = str[i]) != '\0') { // until end-of-line
        str[i] = lower_char(c); // lower each char
        i++;
    }
    return;
}

int main(int argc, char **argv) {

    /* default values */
    uint32_t ht_len = 10000;
    uint32_t bf_len = 1048576; // 2^20

    /* flag parsing */
    enum flags { Stat = 0, Mtf };
    BitVector *args = bv_create(2); // using already made bv instead of set. only 2 args added

    /* for getopt/arg parsing */
    int c;
    char *optlist = "hsmt:f:";

    /* parsing arguments and adding flags */
    while ((c = getopt(argc, argv, optlist)) != -1) {
        switch (c) {
        case 'h':
            usage(argv[0]);
            main_err(args, NULL, NULL);
            return 0;
        case 's': bv_set_bit(args, Stat); break;
        case 'm': bv_set_bit(args, Mtf); break;
        case 't': ht_len = (uint32_t) atoi(optarg); break;
        case 'f': bf_len = (uint32_t) atoi(optarg); break;
        default:
            usage(argv[0]);
            main_err(args, NULL, NULL);
            return -1;
        }
    }

    /* invalid BF or HT sizes */
    if (!bf_len) {
        fprintf(stderr, "Invalid bloom filter size.\n");
        main_err(args, NULL, NULL);
        return -1;
    }

    if (!ht_len) {
        fprintf(stderr, "Invalid hash table size.\n");
        main_err(args, NULL, NULL);
        return -1;
    }

    /* initaliazing ht and bf and handling err */
    HashTable *ht = ht_create(ht_len, bv_get_bit(args, Mtf)); // mtf true if arg bit set
    if (!ht) {
        fprintf(stderr, "Failed to create Hash Table.\n");
        main_err(args, NULL, NULL);
        return -1;
    }

    BloomFilter *bf = bf_create(bf_len);
    if (!bf) {
        fprintf(stderr, "Failed to create Bloom Filter.\n");
        main_err(args, ht, NULL);
        return -1;
    }

    /* read in badspeak and update bloom filter and ht */
    FILE *bad_file = fopen("badspeak.txt", "r");
    if (!bad_file) {
        fprintf(stderr, "Failed to open badspeak.txt file.\n");
        main_err(args, ht, bf);
        return -1;
    }
    read_file(bad_file, ht, bf, true); // call the helper function (true because reading badspeak)

    /* read in newspeak file and update bf and ht */
    FILE *new_file = fopen("newspeak.txt", "r");
    if (!new_file) {
        fprintf(stderr, "Failed to open newspeak.txt file.\n");
        main_err(args, ht, bf);
        return -1;
    }
    read_file(new_file, ht, bf, false); // call the helper function (false because reading newspeak)

    /* read in from stdin and filter the words */

    bool thoughtcrime = false, rightcrime = false; // to track which crime did the citizen commit
    Node *temp = NULL; // to store the hash table entry

    bool print_stats = bv_get_bit(args, Stat); // only do some things below if not printing stats

    /* buffer to store transgressions */
    LinkedList *bad_buf = NULL, *right_buf = NULL;

    // no need for buffer if only printing stats
    bad_buf = ll_create(bv_get_bit(args, Mtf)); // true to make checking for repeated words faster
    right_buf = ll_create(bv_get_bit(args, Mtf));

    /* cannot allocate mem (and not printing stats) */
    if (!bad_buf || !right_buf) {
        fprintf(stderr, "Failed to allocate memory for buffers to store the transgressions.\n");
        ll_delete(&bad_buf);
        ll_delete(&right_buf);
        main_err(args, ht, bf);
        return -1;
    }

    /* compile regex (credits: based on the sample code provided in the lab doc) */
    regex_t re;

    if (regcomp(&re, PATTERN, REG_EXTENDED)) {
        fprintf(stderr, "Failed to compile regex.\n");
        ll_delete(&bad_buf);
        ll_delete(&right_buf);
        main_err(args, ht, bf); // add to main err later
        return -1;
    }

    char *word = NULL; // returned by next_word

    /* start scanning */
    while ((word = next_word(stdin, &re)) != NULL) {

        lower_str(word);

        /* if word is in the bf and in the ht (no false positive) */
        if (bf_probe(bf, word) && (temp = ht_lookup(ht, word))) {

            /* no newspeak translation. citizen committed thoughtcrime */
            if (!(temp->newspeak)) {
                thoughtcrime = true;
                ll_insert(bad_buf, word, NULL);
            }

            /* there is a newspeak entry. counsel on rightcrime */
            else {
                rightcrime = true;
                ll_insert(right_buf, word, temp->newspeak);
            }
        }
    }

    /* if else to avoid repeating free mem code */
    /* print stats (formula credits: given in the lab doc) */
    if (print_stats) {
        fprintf(stdout, "Seeks: %" PRIu64 "\n", seeks);
        fprintf(stdout, "Average seek length: %0.6lf\n", ((double) links) / seeks);
        fprintf(
            stdout, "Hash table load: %0.6lf%%\n", 100 * (((double) ht_count(ht)) / ht_size(ht)));
        fprintf(
            stdout, "Bloom filter load: %0.6lf%%\n", 100 * (((double) bf_count(bf)) / bf_size(bf)));
    }

    /* notify the citizens of their errors */
    else {

        /* mixspeak message if both thoughtcrime and rightcrime */
        if (thoughtcrime && rightcrime) {
            fprintf(stdout, "%s", mixspeak_message);
            ll_print(bad_buf);
            ll_print(right_buf);
        }

        /* only committed thoughtcrime. give badspeak message */
        else if (thoughtcrime && !rightcrime) {
            fprintf(stdout, "%s", badspeak_message);
            ll_print(bad_buf);
        }

        /* only accused of wrongthing. requires counseling. goodspeak message */
        else if (!thoughtcrime && rightcrime) {
            fprintf(stdout, "%s", goodspeak_message);
            ll_print(right_buf);
        }
    }

    /* freeing mem */
    regfree(&re);
    ll_delete(&bad_buf);
    ll_delete(&right_buf);
    main_err(args, ht, bf);

    return 0;
}
