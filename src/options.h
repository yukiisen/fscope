#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct {
    unsigned int preview : 1;
    char *preview_cmd;
    char *prompt;
    char *tty;
    unsigned int gaps : 1;
} CliArgs;

void get_options (CliArgs *options, char *const *argv, const int argc);

#endif
