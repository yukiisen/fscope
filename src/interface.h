#ifndef INTERFACE_H
#define INTERFACE_H

#include <sys/types.h>
#include "options.h"
#include "term.h"
#include "types.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

enum DrawFlags {
    QUERYONLY = 1 << 0,
    PREVIEW = 1 << 1
};

void draw(CliArgs *options, Terminal *term, const Scores *entries, const String *query, int flags);
void selectup();
void selectdown();

#endif
