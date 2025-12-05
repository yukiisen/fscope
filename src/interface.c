#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../config.h"
#include "interface.h"
#include "match.h"
#include "options.h"
#include "term.h"

static void draw_box (Terminal *term, int x, int y, int width, int height, const char *title) {
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

struct layout_t {
    int hgap;
    int vgap;
    int height;
    int width;
    int ent_w;
    int ent_h;
    int preview_w;
    int preview_h;
};

#undef selected // this was defined as a caller to the __selected function, undef it.
static int selected = 0;
static WinSize ws; // this can be used by all draw functions
static struct layout_t layout;

void select_ent (int val) { selected = val; }
int __selected() { return selected; }

// this function is responsible of drawing the borders as well
// as initializing global variables to be used by other functions
void draw_outline(CliArgs *options, Terminal *term) {
    static int original_preview = -1;

    if (original_preview == -1) original_preview = options->preview;

    term_get_size(term, &ws);
    term_clear(term);

    layout.vgap = options->gaps == 0 ? 0 : INTERFACE_GAP_VERTICAL;
    layout.hgap = options->gaps == 0 ? 0 : INTERFACE_GAP_HORIZONTAL;

    layout.height = ws.ws_row - layout.hgap * 2;
    layout.width = ws.ws_col - layout.vgap * 2;

    if (ws.ws_col < 100)
        options->preview = false;
    else options->preview = original_preview;

    layout.ent_w = options->preview == 0 ? layout.width: layout.width * 0.6;
    layout.ent_h = layout.height - 3;

    layout.preview_w = 0;
    layout.preview_h = 0;

    if (options->preview) {
        layout.preview_w = layout.width * 0.4;
        layout.preview_h = layout.height;
    }

    draw_box(term, layout.hgap, layout.vgap, layout.ent_w, layout.ent_h, "Results");
    draw_box(term, layout.hgap + layout.ent_h - 1, layout.vgap, layout.ent_w, 4, "Find");

    if (options->preview)
        draw_box(term, layout.hgap, layout.vgap + layout.ent_w, layout.preview_w, layout.preview_h, "Preview");
}

#define last_idx (entries->length - 1)

void draw_entries(Terminal *term, const Scores *entries) {
    #define drawable_entries (layout.ent_h - 2)

    term_save_cursor(term);

    // clamp selected so that is doesn't do boom boom
    selected = MAX(0, MIN(selected, last_idx));

    int pad = MAX(0, selected - (drawable_entries) + SCROLLOFF + 2);
    if (entries->length - pad < drawable_entries) pad -= (drawable_entries) - (entries->length - pad) - 1;
    pad = MAX(pad, 0);

    for (int i = 0; i < drawable_entries - 1; i++) {
        term_move_cursor(term, layout.hgap + layout.ent_h - i - 3, layout.vgap + 1);
        if (pad + i >= entries->length) {
            for (int j = 0; j < layout.ent_w - 2; j++)
                term_put(term, ' ');
            continue;
        }

        if ((pad + i) == selected) term_inverse(term);
        else term_normal(term);

        uint writing = true; // whether we should write or fill with whitespace
        for (int j = 0; j < layout.ent_w - 2; j++) {
            if (writing == false)
                term_put(term, ' ');
            else if (entries->pairs[pad + i].entry[j] == '\0') {
                writing = false;
                term_put(term, ' ');
            } else
                term_put(term, entries->pairs[pad + i].entry[j]);
        }

        if (i == selected) term_normal(term);
    }

    term_restore_cursor(term);

    #undef drawable_entries
}

void draw_query(CliArgs *options, Terminal *term, const String *query) {
    static int view = 0; // the view should have an independent state

    term_move_cursor(term, layout.hgap + layout.ent_h, layout.vgap + 1);
    term_write(term, options->prompt);

    const int p_len = strlen(options->prompt);

    #define available_space (layout.ent_w - 3 - p_len)
    
    if (query->cursor_pos > view + available_space) view = query->cursor_pos - available_space;
    else if (query->cursor_pos < view) view = query->cursor_pos;

    term_write_buf(term, query->bytes + view, MIN(layout.ent_w - 3 - p_len, query->length));

    if (query->length + p_len < layout.ent_w - 3) {
        for (int i = 0; i < layout.ent_w - 2 - query->length - p_len; i++) term_put(term, ' ');
    }

    term_move_cursor(term, layout.hgap + layout.ent_h, layout.vgap + (query->cursor_pos - view) + 1 + p_len);

    #undef available_space
}

void draw_preview(CliArgs *options, Terminal *term, const Scores *entries) {
    if (!options->preview) return;
    term_save_cursor(term);

    // clamp selected so that is doesn't do boom boom
    selected = MAX(0, MIN(selected, last_idx));

    char c;
    int x = layout.hgap + 1;
    int y = layout.vgap + layout.ent_w + 1;
    int width = layout.preview_w - 3;
    int height = layout.preview_h - 4;
    int row = x; // this is weird but that's how things work ._.
    int col = y; // I'll fix it once I have braincells

    term_move_cursor(term, x, y);

    if (entries->length == 0) goto clear_screen;

    String command;
    str_replace(options->preview_cmd, "{}", entries->pairs[selected].entry, &command);

    FILE *preview = popen(command.bytes, "r");

    term_move_cursor(term, row, col);
    while ((c = fgetc(preview)) != EOF) {
        if (c != '\n' && c != '\t') term_put(term, c);
        if (c == '\t') {
            col += 4;
            for (uint i = 0; i < 4; i++) term_put(term, ' ');
        }
            
        if (col++ == y + width || c == '\n') {
            if (c == '\n') 
                while (col++ <= y + width + 1) term_put(term, ' ');
            col = y;
            term_move_cursor(term, ++row, col);
        }

        if (row == x + height) break;
    }

    pclose(preview);

clear_screen:
    while (row < x + height) {
        while (col++ <= y + width) term_put(term, ' ');
        col = y;
        term_move_cursor(term, ++row, col);
    }

    term_restore_cursor(term);
}
