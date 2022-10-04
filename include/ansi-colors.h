#ifndef ANSI_COLORS_H
#define ANSI_COLORS_H

typedef struct ANSI {
    char *reset;
    char *black;
    char *white;
    char *red;
    char *green;
    char *blue;
    char *yellow;
} ANSI;

// Escape characters are: \x1b, \u001B \e
extern ANSI ansi;

#endif
