#include <stddef.h>
#include <string.h>

#include "interface.h"
#include "../config.h"
#include "term.h"

static void draw_box (Terminal *term, int x, int y, int width, int height, const char *title) {
    // TODO: try to replace the write calls with fputc calls
    term_move_cursor(term, x, y);

    term_write(term, BORDER_TOP_LEFT);

    int t_len = strlen(title) - 1;
    int startpos = (width - t_len) / 2 - 2;

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

void draw (CliArgs *options, Terminal *term, const Scores *entries, const String *query) {
    WinSize ws;
    term_get_size(term, &ws);

    term_clear(term);

    const int vgap = options->gaps == 0 ? 0 : INTERFACE_GAP_VERTICAL;
    const int hgap = options->gaps == 0 ? 0 : INTERFACE_GAP_HORIZONTAL;

    int height = ws.ws_row - hgap * 2;
    int width = ws.ws_col - vgap * 2;

    if (ws.ws_col < 100)
        options->preview = 0;

    int ent_w = options->preview == 0 ? width: width * 0.6;
    int ent_h = height - 3;

    int preview_w = 0;
    int preview_h = 0;

    if (options->preview) {
        preview_w = options->preview == 0 ? 0 : width * 0.4;
        preview_h = height;
    }

    draw_box(term, hgap, vgap, ent_w, ent_h, "Results");
    draw_box(term, hgap + ent_h - 1, vgap, ent_w, 4, "Find");

    if (options->preview)
        draw_box(term, hgap, vgap + ent_w, preview_w, preview_h, "Preview");
    
    if (selected < 0) selected = 0;
    else if (selected > entries->length - 1) selected = entries->length - 1;

    int pad = MAX(0, selected - (ent_h - 2) + SCROLLOFF + 2);
    if (pad != 0 && (int)entries->length - pad < ent_h - 2) pad -= (ent_h - 2) - (entries->length - pad) - 1;

    for (int i = pad; i < MIN(entries->length, pad + ent_h - 3); i++) {
        term_move_cursor(term, hgap + ent_h - 3 - (i - pad), vgap + 1);
        if (i == selected) term_inverse(term);

        uint writing = true;
        for (int j = 0; j < ent_w - 1; j++) {
            if (writing == false)
                term_put(term, ' ');
            else if (entries->pairs[i].entry[j] == '\0')
                writing = false;
            else
                term_put(term, entries->pairs[i].entry[j]);
        }

        if (i == selected) term_normal(term);
    }

    term_move_cursor(term, hgap + ent_h, vgap + 1);
    term_write(term, options->prompt);

    const int p_len = strlen(options->prompt);
    if (query->length + p_len > ent_w - 2)
        term_write(term, query->bytes + query->length - ent_w + 3 + p_len);
    else
        term_write(term, query->bytes);


    term_flush(term);
}
