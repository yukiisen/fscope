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
void term_write_buf (Terminal *term, const char *buf, size_t len);

int term_read(Terminal *term);
int term_read_buf (Terminal *term, void *buf, size_t len);

Terminal* create_term();
void destroy_term(Terminal *term);

inline void term_flush (Terminal *term) {
    fflush(term->output);
}

inline void term_clear (Terminal *term) {
    term_write(term, "\x1b[2J");
}

inline void term_save_cursor (Terminal *term) {
    term_write(term, "\x1b[s");
}

inline void term_restore_cursor (Terminal *term) {
    term_write(term, "\x1b[u");
}

inline void term_inverse (Terminal *term) {
    term_write(term, "\x1b[7m");
}

inline void term_normal (Terminal *term) {
    term_write(term, "\x1b[0m");
}

inline void term_put(Terminal *term, int c) {
    fputc(c, term->output);
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
