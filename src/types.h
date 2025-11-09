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

struct pair {
    score_t score;
    char *entry;
};

typedef struct {
    struct pair pairs[MAX_MATCHES];
    int length;
} Scores;

typedef struct {
    int length;
    char bytes[MAX_TEXT_LEN];
} String;

Entries *list_create (size_t capacity);
int list_append (Entries *list, const char *buffer, size_t len);
void list_free (Entries *list);

int cmp_pairs (const void *p1, const void *p2);

#endif

#ifdef LIST_IMPL
int cmp_pairs (const void *p1, const void *p2) {
    struct pair left = *(const struct pair *)p1;
    struct pair right = *(const struct pair *)p2;

    return (left.score > right.score) - (left.score < right.score);
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
