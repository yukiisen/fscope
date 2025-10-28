#ifndef BONUS_H
#define BONUS_H

#include "../config.h"
#include "match.h"

// #define ASSIGN_LOWER(v) \
// 	['a'] = (v), \
// 	['b'] = (v), \
// 	['c'] = (v), \
// 	['d'] = (v), \
// 	['e'] = (v), \
// 	['f'] = (v), \
// 	['g'] = (v), \
// 	['h'] = (v), \
// 	['i'] = (v), \
// 	['j'] = (v), \
// 	['k'] = (v), \
// 	['l'] = (v), \
// 	['m'] = (v), \
// 	['n'] = (v), \
// 	['o'] = (v), \
// 	['p'] = (v), \
// 	['q'] = (v), \
// 	['r'] = (v), \
// 	['s'] = (v), \
// 	['t'] = (v), \
// 	['u'] = (v), \
// 	['v'] = (v), \
// 	['w'] = (v), \
// 	['x'] = (v), \
// 	['y'] = (v), \
// 	['z'] = (v)

// #define ASSIGN_UPPER(v) \
// 	['A'] = (v), \
// 	['B'] = (v), \
// 	['C'] = (v), \
// 	['D'] = (v), \
// 	['E'] = (v), \
// 	['F'] = (v), \
// 	['G'] = (v), \
// 	['H'] = (v), \
// 	['I'] = (v), \
// 	['J'] = (v), \
// 	['K'] = (v), \
// 	['L'] = (v), \
// 	['M'] = (v), \
// 	['N'] = (v), \
// 	['O'] = (v), \
// 	['P'] = (v), \
// 	['Q'] = (v), \
// 	['R'] = (v), \
// 	['S'] = (v), \
// 	['T'] = (v), \
// 	['U'] = (v), \
// 	['V'] = (v), \
// 	['W'] = (v), \
// 	['X'] = (v), \
// 	['Y'] = (v), \
// 	['Z'] = (v)

// #define ASSIGN_DIGIT(v) \
// 	['0'] = (v), \
// 	['1'] = (v), \
// 	['2'] = (v), \
// 	['3'] = (v), \
// 	['4'] = (v), \
// 	['5'] = (v), \
// 	['6'] = (v), \
// 	['7'] = (v), \
// 	['8'] = (v), \
// 	['9'] = (v)

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
