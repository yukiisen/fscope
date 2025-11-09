#include <ctype.h>
#include <string.h> 
#include "match.h"
#include "bonus.h"

int has_match(const char *search, const char *entry) {
    while(*search) {
        char c = *search++;
        char options[3] = {toupper(c), tolower(c), 0};

        if (!(entry = strpbrk(entry, options)))
            return 0;

        entry++;
    }

    return 1;
}

static inline void calculate_bonus (const char *entry, score_t *bonus_list) {
    char last_ch = '/';

    for (uint i = 0; entry[i]; i++) {
        char ch = entry[i];
        bonus_list[i] = calc_bonus(last_ch, ch);
        last_ch = ch;
    }
}

score_t score_match(const char *search, const char *entry) {
    MatchStruct match = {0};
    
    match.search_len = strlen(search);
    match.entry_len = strlen(entry);

    if (match.search_len > MAX_TEXT_LEN || match.entry_len > MAX_TEXT_LEN) {
        return MINSCORE;
    }

    for (uint i = 0; i < match.search_len; i++) 
        match.lower_search[i] = tolower(search[i]);

    for (uint i = 0; i < match.entry_len; i++) 
        match.lower_entry[i] = tolower(entry[i]);

    calculate_bonus(entry, match.bonus);

    uint n = match.search_len;
    uint m = match.entry_len;

    if (!n) return MAXSCORE;
    if (n == m) return MAXSCORE;

    uint i = 0, j = 0;
    uint last_j = 0;
    score_t score = 0;

    while (i < n) {
        if (match.lower_search[i] == match.lower_entry[j]) {
            if (!i) {
                score += (j * SCORE_GAP_LEADING) + match.bonus[j];
            } else {
               score += SCORE_MATCH_CONSECUTIVE + (j - last_j) * SCORE_GAP_INNER + match.bonus[j];
            }
    
            last_j = j;
            i++;
        }

        j++;
    }

    score += (match.entry_len - j) * SCORE_GAP_TRAILING;

    return score;
}
