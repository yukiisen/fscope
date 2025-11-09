#ifndef BONUS_H
#define BONUS_H

#include "../config.h"
#include "match.h"

#define ASSIGN_UPPER(v) [ 'A' ... 'Z' ] = v
#define ASSIGN_LOWER(v) [ 'a' ... 'z' ] = v
#define ASSIGN_DIGIT(v) [ '0' ... '9' ] = v

const score_t bonus[2][256] = {
    {
		['/'] = SCORE_MATCH_SLASH,
		['-'] = SCORE_MATCH_WORD,
		['_'] = SCORE_MATCH_WORD,
		[' '] = SCORE_MATCH_WORD,
		['.'] = SCORE_MATCH_DOT,
    }, {
		['/'] = SCORE_MATCH_SLASH,
		['-'] = SCORE_MATCH_WORD,
		['_'] = SCORE_MATCH_WORD,
		[' '] = SCORE_MATCH_WORD,
		['.'] = SCORE_MATCH_DOT,

        ASSIGN_LOWER(SCORE_MATCH_CAPITAL)
    }
};

const size_t bonus_idx[256] = {
    ASSIGN_UPPER(1),
    ASSIGN_LOWER(0),
    ASSIGN_DIGIT(0),
};

typedef unsigned char uchar;

static inline score_t calc_bonus(const char prev_ch, const char curr_ch) {
    return bonus[bonus_idx[(uchar)curr_ch]][(uchar)prev_ch];
}

#endif
