#ifndef MATCH_H
#define MATCH_H

#include <sys/types.h>
#include <math.h>

#define MINSCORE -INFINITY
#define MAXSCORE INFINITY
#define MAX_TEXT_LEN 1024

typedef double score_t;

typedef struct {
    uint search_len;
    uint entry_len;

    char lower_search[MAX_TEXT_LEN];
    char lower_entry[MAX_TEXT_LEN];

    score_t bonus[MAX_TEXT_LEN];
} MatchStruct;


int has_match(const char *search, const char *entry);
score_t score_match(const char *search, const char *entry);

#endif
