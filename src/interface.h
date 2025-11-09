#ifndef INTERFACE_H
#define INTERFACE_H

#include <sys/types.h>
#include "options.h"
#include "term.h"
#include "types.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

static int selected = 0;
void draw(CliArgs *options, Terminal *term, const Scores *entries, const String *query);

#endif
