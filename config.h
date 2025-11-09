#ifndef CONFIG_H
#define CONFIG_H

// penality
#define SCORE_GAP_LEADING -0.005
#define SCORE_GAP_TRAILING -0.005
#define SCORE_GAP_INNER -0.01

// bonus
#define SCORE_MATCH_CONSECUTIVE 0.9
#define SCORE_MATCH_SLASH 0.8
#define SCORE_MATCH_WORD 0.7
#define SCORE_MATCH_CAPITAL 0.6
#define SCORE_MATCH_DOT 0.5

#define SCORING_STEP 20

// ui stuff
#define INPUT_TIMEOUT 50 // in ms
#define INTERFACE_GAP_VERTICAL 8
#define INTERFACE_GAP_HORIZONTAL 4
#define SCROLLOFF 10

#define DEFAULT_PROMPT "> "
#define DEFAULT_TTY "/dev/tty"
#define FALLBACK_COMMAND "ls"
#define DEFAULT_PREVIEW "cat {}"

#define BORDER_TOP_LEFT     "┌"
#define BORDER_TOP_RIGHT    "┐"
#define BORDER_BOTTOM_LEFT  "└"
#define BORDER_BOTTOM_RIGHT "┘"
#define BORDER_HORIZONTAL   "─"
#define BORDER_VERTICAL     "│"

#endif
