#ifndef INTERFACE_H
#define INTERFACE_H

#include <sys/types.h>
#include "options.h"
#include "term.h"
#include "types.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

// deprecated
// void draw(CliArgs *options, Terminal *term, const Scores *entries, const String *query);
// new drawing functions
void draw_outline(CliArgs *options, Terminal *term);
void draw_entries(Terminal *term, const Scores *entries);
void draw_preview(CliArgs *options, Terminal *term, const Scores *entries);
void draw_query(CliArgs *options, Terminal *term, const String *query);
int __selected();
void select_ent(int val);

#define selected (__selected())

#endif
