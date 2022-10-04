#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char *read_file(const char *path);

int get_file_length(const char *path);

#endif

