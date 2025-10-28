#include <asm-generic/ioctls.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "term.h"
#include "../config.h"

void term_write (Terminal *term, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(term->output, fmt, args);
	va_end(args);
}

char term_read (Terminal *term) {
    char buf[1];
    read(term->input, buf, 1);

    return buf[0];
}

static inline void enter_alternate_buffer (FILE *fd) {
    fputs("\x1b[?1049h", fd);
}

static inline void leave_alternate_buffer (FILE *fd) {
    fputs("\x1b[?1049l", fd);
}

Terminal * create_term() {
    Terminal * term = malloc(sizeof(Terminal));

    term->input = open(DEFAULT_TTY, O_RDONLY);
    if (term->input == -1) return NULL;

    term->output = fopen(DEFAULT_TTY, "w");
    if (!term->output) return NULL;
    
    if (setvbuf(term->output, NULL, _IOFBF, 1024 * 16) != 0) return NULL; // apply buffering

    if (tcgetattr(term->input, &term->original_ios)) return NULL;

    struct termios new_termios = term->original_ios;

	new_termios.c_iflag &= ~(ICRNL);
	new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

    if (tcsetattr(term->input, TCSANOW, &new_termios)) return NULL;

    enter_alternate_buffer(term->output); // move to alternate screen
    term_flush(term);

    return term;
}

void destroy_term (Terminal *term) {
    if (!term) return;
    leave_alternate_buffer(term->output);
    fflush(term->output);
    tcsetattr(term->input, TCSANOW, &term->original_ios);
    fclose(term->output);
    close(term->input);
    free(term);
}
