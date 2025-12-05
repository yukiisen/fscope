#ifndef FTYPES_H
#define FTYPES_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "match.h"

// Note that I hate header-only libs, but I'll accept this since the file is small.

#define false 0
#define true 1

#define MAX_MATCHES 2048

typedef struct {
    char **items;
    int length;
    int capacity;
} Entries;

struct pair_t {
    score_t score;
    char *entry; // pointer from Entries
};

typedef struct {
    struct pair_t pairs[MAX_MATCHES];
    int length;
} Scores;

typedef struct {
    int length;
    int cursor_pos;
    char bytes[MAX_TEXT_LEN];
} String;

Entries *list_create (size_t capacity);
int list_append (Entries *list, const char *buffer, size_t len);
void list_free (Entries *list);

int cmp_pairs (const void *p1, const void *p2);

// Shifts bytes starting from `cursor_pos` by `amount`.
inline int shift_string (String *str, ssize_t amount) {
    // return false if the operation overflows buffer
    if (str->length + amount > MAX_TEXT_LEN) return false; 
    if (str->length + amount < 0) return false;

    if (amount == 0) return true;
    else if (amount > 0) 
        for (int i = str->length; i >= str->cursor_pos; i--) str->bytes[i + amount] = str->bytes[i]; // shift forward
    else 
        for (int i = str->cursor_pos; i < str->length + 1; i++) if (i + amount >= 0) str->bytes[i + amount] = str->bytes[i]; // shift backward

    str->length += amount;

    return true;
}

#endif

#ifdef LIST_IMPL
int cmp_pairs (const void *p1, const void *p2) {
    struct pair_t left = *(const struct pair_t *)p1;
    struct pair_t right = *(const struct pair_t *)p2;

    return (left.score < right.score) - (left.score > right.score);
}

Entries *list_create (size_t capacity) {
    Entries *list = malloc(sizeof(Entries));
    if (list == NULL) return NULL;
    
    list->capacity = capacity;
    list->items = malloc(capacity * sizeof(char *));
    if (list->items == NULL) return NULL;

    return list;
}

int list_append (Entries *list, const char *buffer, size_t len) {
    if (list->length == list->capacity) {
        list->capacity = list->capacity * 2 + 1; // increase the list's capacity
        list->items = realloc(list->items, list->capacity * sizeof(char *)); // reallocate

        if (list->items == NULL) return -1;
    }

    list->items[list->length] = malloc(len * sizeof(char));
    if (list->items[list->length] == NULL) return -1;

    for (uint i = 0; i < len; i++)
        list->items[list->length][i] = buffer[i];

    list->length += 1;

    return 0;
}

void list_free (Entries *list) {
    for (int i = 0; i < list->length; i++) {
        free(list->items[i]);
    }

    free(list->items);
    free(list);
}
#endif
