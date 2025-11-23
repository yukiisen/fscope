#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "interface.h"
#include "match.h"
#include "term.h"

static int selected = 0;

void selectup () {
    selected++;
}

void selectdown () {
    selected--;
}

static void draw_box (Terminal *term, int x, int y, int width, int height, const char *title) {
    // TODO: try to replace the write calls with fputc calls
    term_move_cursor(term, x, y);

    term_write(term, BORDER_TOP_LEFT);

    int t_len = strlen(title) - 1;
    int startpos = (width - t_len) / 2 - 2;

    for (int i = 0; i < width - 2; i++) {
        if (i == startpos - 1 || i == startpos + t_len + 1)
            term_put(term, ' ');
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

static int draw_preview (Terminal *term, int x, int y, int width, int height, const char *command) {
    term_move_cursor(term, x, y);

    FILE *preview = popen(command, "r");

    char c;
    int row = x; // this is weird but that's how things work ._.
    int col = y; // I'll fix it once I have braincells

    term_move_cursor(term, row, col);
    while ((c = fgetc(preview)) != EOF) {
        if (c != '\n') term_put(term, c);
        if (col++ == y + width || c == '\n') {
            col = y;
            term_move_cursor(term, ++row, col);
        };

        if (row == x + height) break;
    }

    pclose(preview);

    return 0;
}

// returns -1 one failture and 0 on success
static int str_replace (const char *input, const char *search, const char *replace, String *output) {
    const int search_l = strlen(search);
    char const *rep_orig = replace;
    output->length = 0;
    output->bytes[0] = 0;

    while (*input) {
        if (strncmp(input, search, search_l) == 0) {
            input += search_l;
            while (*replace) {
                output->bytes[output->length++] = *replace++;
                if (output->length == MAX_TEXT_LEN - 1) return -1;
            }
            replace = rep_orig;
        } else {
            output->bytes[output->length++] = *input++;
            if (output->length == MAX_TEXT_LEN - 1) return -1;
        }
    }

    output->bytes[output->length] = '\0';

    return 0;
}
;
void draw (CliArgs *options, Terminal *term, const Scores *entries, const String *query, int flags) {
    WinSize ws;
    term_get_size(term, &ws);

    term_clear(term); // TODO: this is not opengl, stop clearing

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

    if (options->preview && entries->length > 0 && (flags & PREVIEW)) {
        String command;
        str_replace(options->preview_cmd, "{}", entries->pairs[selected].entry, &command);
        draw_preview(term, hgap + 1, vgap + ent_w + 1, preview_w - 3, preview_h - 4, command.bytes);
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
