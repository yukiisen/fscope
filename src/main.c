#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <term.h>
#include <unistd.h>

#include "../config.h"
#include "interface.h"
#include "term.h"

ssize_t readline (FILE *stream, char *buf, ssize_t maxlen) {
    ssize_t i = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        if (c == '\n')
            break;

        if (i < maxlen - 1)
            buf[i++] = (char)c;
        else 
            break;
    }

    buf[i] = '\0';

    if (i == 0 || c == EOF)
        return -1;

    return i;
}

FILE *open_input_stream () {
    if (isatty(STDIN_FILENO)) {
        // no input piped, use a preset command.
        FILE *cmd_pipe = popen(FALLBACK_COMMAND, "r");

        if (!cmd_pipe) {
            perror("Couldn't create fallback pipe.");
            exit(1);
        }
        
        return cmd_pipe;
    } else {
        return stdin;
    }
}

void close_input_stream (FILE *stream) {
    if (isatty(STDIN_FILENO)) {
        pclose(stream);
    }
}

int main() {
    FILE *input = open_input_stream();
    Terminal *term = create_term();

    draw(term);

    destroy_term(term);
    close_input_stream(input);

    return 0;
}
