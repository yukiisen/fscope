#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <wait.h>

#include "../config.h"
#include "interface.h"
#include "match.h"
#include "options.h"
#include "term.h"

#define LIST_IMPL
#include "types.h"

#define CTRL_KEY(key) (key - 'a' + 1)
#define IS_CHAR(c) (c < 127 && c > 31)
#define BACKSPACE 127
#define ENTER 13

typedef struct {
    int fd;
    int pid;
} Proc;

// NOTE: static variables reduce stack allocations, use when possible
static String query = {0};
static CliArgs options = {0};
static Scores matches = {0};
static Proc input = {0};

// input buffer
static char i_buf[MAX_TEXT_LEN];

ssize_t readline (int fd, char *buf, ssize_t maxlen);
int open_input_stream(Proc *p);
void close_input_stream (Proc *p);

int main(int argc, char **argv) {
    get_options(&options, argv, argc);
    open_input_stream(&input);

    Terminal *term = create_term();
    Entries *entries = list_create(10);

    struct {
        struct pollfd term;
        struct pollfd input;
    } fds; // this is the same as pollfd fds[2] but allows for key indexing :)

    fds.term.fd = term->input;
    fds.term.events = POLLIN;

    fds.input.fd = input.fd;
    fds.input.events = POLLIN;
    
    int p_ret;
    int p_timeout = -1; // non blocking poll

    int midx = 0;

    nfds_t nfds = 2;

    while ((p_ret = poll(&fds.term, nfds, p_timeout)) >= 0) {
        if (p_ret == 0) {
            // nothing was polled, do scoring
            // we score 10 entries before each time

            if (midx == entries->length) {
                draw(&options, term, &matches, &query);
                p_timeout = -1;
                continue;
            }

            for (const int i = midx + SCORING_STEP; midx < i && midx < entries->length; midx++) {
                if (has_match(query.bytes, entries->items[midx])) {
                    score_t score = score_match(query.bytes, entries->items[midx]);
                    matches.pairs[matches.length].entry = entries->items[midx];
                    matches.pairs[matches.length].score = score;
                    matches.length++;
                }
            }

            qsort(matches.pairs, matches.length, sizeof(struct pair), cmp_pairs);

            p_timeout = midx == entries->length? -1 : 0; // if no more jobs do a blocking poll, otherwise disable timeout

            draw(&options, term, &matches, &query);

            continue;
        }

        if (fds.term.revents & POLLIN) {
            // key press
            int c = term_read(term);

            if (IS_CHAR(c)) {
                query.bytes[query.length++] = c; // push character to query
                query.bytes[query.length] = 0;

                // reset entries
                midx = 0;
                matches.length = 0;
            }

            switch (c) {
                case BACKSPACE:
                    query.length = MAX(query.length - 1, 0);
                    query.bytes[query.length] = 0; // remove last character from query
                    
                    // reset entries
                    midx = 0;
                    matches.length = 0;
                    break;

                case CTRL_KEY('n'):
                    selected--;
                    break;

                case CTRL_KEY('p'):
                    selected++;
                    break;
                case ENTER:
                    goto loop_end;
                    break;
            }

            p_timeout = INPUT_TIMEOUT;
        } else {
            p_timeout = 0;
        }

        if ((fds.input.revents & POLLHUP) && !(fds.input.revents & POLLIN)) {
            nfds = 1;
            fds.input.revents = 0;
        }

        if (fds.input.revents & POLLIN) {
            // new line
            ssize_t n = readline(input.fd, i_buf, MAX_TEXT_LEN);

            if (n == EOF) {
                nfds = 1;
                fds.input.revents = 0;
            } 
            else if (n == -2) perror("fuck something broke!");
            else if (n > 0) list_append(entries, i_buf, n); // don't add empty strings

            p_timeout = MAX(0, p_timeout);
        }
    }

loop_end:

    if (p_ret < 0) {
        perror("Poll");
    }

    list_free(entries);
    destroy_term(term);
    close_input_stream(&input);

    return 0;
}


ssize_t readline (int fd, char *buf, ssize_t maxlen) {
    ssize_t i = 0;
    int c;
    int ret;

    while ((ret = read(fd, &c, 1)) > 0) {
        if (c == '\n')
            break;

        if (i < maxlen - 1)
            buf[i++] = (char)c;
        else 
            break;
    }

    buf[i] = '\0';

    if (ret == 0)
        return EOF;

    if (ret < 0) return -2;

    return i;
}

int open_input_stream (Proc *p) {
    if (isatty(STDIN_FILENO)) { // no input piped, use a preset command.
        int fds[2]; // 0 read, 1 write
        int ret = pipe(fds);

        if (ret == -1) {
            perror("pipe");
            return -1;
        }

        ret = fork();

        if (ret == 0) {
            // new proc
            dup2(fds[1], STDOUT_FILENO);
            close(fds[0]);

            static char *const argv[] = { "/bin/sh", "-c", FALLBACK_COMMAND, NULL };

            execvp("/bin/sh", argv); // change this to use shell
            perror("execvp");
            return -1; // this should be unreachable
        } else if (ret > 0) {
            // parent proc
            close(fds[1]);

            p->fd = fds[0];
            p->pid = ret;
            return 0;
        } else {
            perror("fork");
            return -1;
        }
    } else {
        p->fd = STDIN_FILENO;
        p->pid = -1;
        return 0;
    }
}

void close_input_stream (Proc *p) {
    if (isatty(STDIN_FILENO)) {
        close(p->fd);
        int status;
        waitpid(p->pid, &status, 0); // consider killing the process here
    }
}
