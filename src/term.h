#ifndef TERM_H
#define TERM_H 

#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

typedef struct {
    int input;
    FILE *output;

    struct termios original_ios;
} Terminal;

typedef struct { 
    int16_t ws_row;
    int16_t ws_col;
    int16_t _reserved[2];
} WinSize;

void term_write(Terminal *term, const char *fmt, ...);
char term_read(Terminal *term);

Terminal* create_term();
void destroy_term(Terminal *term);

inline void term_flush (Terminal *term) {
    fflush(term->output);
}

inline void term_move_cursor (Terminal *term, int x, int y) {
    term_write(term, "\x1b[%d;%dH", x + 1, y + 1);
}

inline void term_hide_cursor (Terminal *term) {
    term_write(term, "\x1b[?25l");
}

inline void term_show_cursor (Terminal *term) {
    term_write(term, "\x1b[?25h");
}

inline void term_get_size (Terminal *term, WinSize *ws) {
    ioctl(term->input, TIOCGWINSZ, ws);
}

#endif
