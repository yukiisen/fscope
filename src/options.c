#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "options.h"
#include "../config.h"

#ifndef VERSION
#define VERSION "0.0.1"
#endif

void get_options (CliArgs *options, char *const *argv, int argc) {
    char *arg = NULL;

    for (int i = 1; i < argc; i++) {
        arg = argv[i];
        if (strcmp("--preview", arg) == 0) {
            options->preview = 1;

            if (i < argc - 1 && argv[i + 1][0] != '-') {
                options->preview_cmd = argv[++i];
            }

        } else if (strcmp("--gaps", arg) == 0) {
            options->gaps = 1; // just set the flag.
            
        } else if (strcmp("--prompt", arg) == 0) {
            if (i < argc - 1)
                options->prompt = argv[++i];
            else goto usage;

        } else if (strcmp("--tty", arg) == 0) {
            if (i < argc - 1)
                options->tty = argv[++i];
            else goto usage;
        } else if (strcmp("--version", arg) == 0) {
            printf("fscope-" VERSION "\n");
            exit(0);
        } else {
            goto usage;
        }
    }

    if (options->tty == NULL)
        options->tty = DEFAULT_TTY;

    if (options->prompt == NULL)
        options->prompt = DEFAULT_PROMPT;

    if (options->preview_cmd == NULL)
        options->preview_cmd = DEFAULT_PREVIEW;

    return;
usage:
    fprintf(stderr, "Usage %s [--version] [--preview [PREVIEW]] [--prompt PROMPT] [--tty DEFAULT_TTY] [--gaps]\n", argv[0]);
    exit(-1);
}
