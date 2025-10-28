#include "interface.h"
#include "term.h"
#include "../config.h"
#include <string.h>

static void draw_box (Terminal *term, int x, int y, int width, int height, const char *title) {
    term_move_cursor(term, x, y);

    term_write(term, BORDER_TOP_LEFT);

    int t_len = strlen(title) - 1;
    int startpos = (width - t_len) / 2 - 1;

    for (int i = 0; i < width - 2; i++) {
        if (i == startpos - 1 || i == startpos + t_len + 1)
            term_write(term, " ");
        else if (i < startpos || i > startpos + t_len)
            term_write(term, BORDER_HORIZONTAL);
        else {
            term_write(term, title);
            i += t_len;
        }
    }

    term_write(term, BORDER_TOP_RIGHT);

    term_move_cursor(term, x + height - 2, y);
    term_write(term, BORDER_BOTTOM_LEFT);

    for (int i = 0; i < width - 2; i++) {
        term_write(term, BORDER_HORIZONTAL);
    }

    term_write(term, BORDER_BOTTOM_RIGHT);

    for (int i = 1; i < height - 2; i++) {
        term_move_cursor(term, x + i, y);
        term_write(term, BORDER_VERTICAL);
    }

    for (int i = 1; i < height - 2; i++) {
        term_move_cursor(term, x + i, y + width - 1);
        term_write(term, BORDER_VERTICAL);
    }
}

void draw (Terminal *term) {
    WinSize ws;
    term_get_size(term, &ws);

    #ifdef DEFAULT_PREVIEW
        int preview = 1;
    #else
        int preview = 0;
    #endif

    int height = ws.ws_row - INTERFACE_GAP_HORIZONTAL * 2;
    int width = ws.ws_col - INTERFACE_GAP_VERTICAL * 2;

    // fprintf(stderr, "%d:%d\n", height, ws.ws_row);
    // fprintf(stderr, "%d:%d\n", width, ws.ws_col);

    if (ws.ws_col < 100)
        preview = 0;

    int ent_w = preview == 0? width: width * 0.6;
    int ent_h = height - 3;

    // TODO: replace this with a runtime flag
    #ifdef DEFAULT_PREVIEW
        int preview_w = preview == 0? 0 : width * 0.4;
        int preview_h = height;
    #endif

    draw_box(term, INTERFACE_GAP_HORIZONTAL, INTERFACE_GAP_VERTICAL, ent_w, ent_h, "Results");
    draw_box(term, INTERFACE_GAP_HORIZONTAL + ent_h - 1, INTERFACE_GAP_VERTICAL, ent_w, 4, "Preview");
    draw_box(term, INTERFACE_GAP_HORIZONTAL, INTERFACE_GAP_VERTICAL + ent_w, preview_w, preview_h, "Find");

    term_hide_cursor(term);
    term_flush(term);

    while (term_read(term) != 'q') {}

    term_show_cursor(term);
}

